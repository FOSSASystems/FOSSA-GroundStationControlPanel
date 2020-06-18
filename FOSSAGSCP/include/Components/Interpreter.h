#ifndef INTERPRETER_H
#define INTERPRETER_H


#include <cstring>
#include <stdexcept>

#include <FOSSA-Comms.h>

#include "Interfaces/IInterpreter.h"

#include "Components/GroundStationSerialMessage.h"
#include "Components/MessageLog.h"
#include "Components/Settings.h"

/////////////////////
/// Direction Bit ///
/////////////////////
#define FCPI_DIR_TO_GROUND_STATION  0
#define FCPI_DIR_FROM_GROUND_STATION 1

////////////////////
/// Operation ID ///
////////////////////
//
// Handshake request/response. Sent from control panel to initialize the serial protocol,
// ground station sends this operation ID in response.
//
#define FCPI_HANDSHAKE_OP 0 // handshake

//
// FCP frame transfer. Payload is only the FCP frame to uplink when sent with direction = 0,
// or 2-byte RadioLib reception status code followed by the received FCP frame when sent with direction = 1.
//
#define FCPI_FRAME_OP 1 // fcp frame

//
// Ground station configuration change request/result. Payload is the ground station configuration to set when sent with direction = 0,
// or configuration result when sent with direction = 1 (typically a 2-byte RadioLib status code).
//
#define FCPI_CONFIG_OP 2







class Interpreter : public IInterpreter
{
public:
    virtual ~Interpreter() {};

    void RegisterSettings(ISettings *settings) override
    {
        m_settings = dynamic_cast<Settings*>(settings);
    }

    /// The ground station serial message is an internal command structure.
    /// This creates it from raw serial datagrams.
    /// @todo checks on the serial data length.
    virtual IGroundStationSerialMessage* SerialData_To_GroundStationSerialMessage(const char* serialData, char serialDataLength) override
    {
        // first byte is the control byte.
        char controlByte = serialData[0];

        // second byte is the payload length.
        char lengthByte = serialData[1];

        // rest of the bytes is the payload.
        char* payload = new char[lengthByte];
        memcpy(payload, &(serialData[2]), lengthByte);

        GroundStationSerialMessage* message = new GroundStationSerialMessage(controlByte, lengthByte, payload);

        // delete the payload, it is copied in the datagram ctor.
        delete[] payload;

        return dynamic_cast<IGroundStationSerialMessage*>(message);
    }

    // this is the base class for all the Create_CMD_XXX methods.
    virtual IGroundStationSerialMessage* Create_GroundStationSerialMessage(char operationId, uint8_t functionId, uint8_t optDataLength, char* optData) override
    {
        char directionBit = FCPI_DIR_TO_GROUND_STATION; // all outgoing messages are to the ground station.

        // check whether this function id is for encryption.
        bool encrypt = functionId >= PRIVATE_OFFSET;

        //
        // Get information
        //
        std::string callsignStr = m_settings->GetCallsign();
        std::string passwordStr = m_settings->GetPassword();

        const char* callsign = callsignStr.c_str();
        const char* password = passwordStr.c_str();
        const uint8_t* key = m_settings->GetKey();

        //
        // Check that the encryption is setup.
        //
        if (encrypt)
        {
            bool passwordSet = m_settings->IsPasswordSet();
            bool keySet = m_settings->IsKeySet();

            if ( !passwordSet || !keySet )
            {
                return nullptr;
            }
        }

        //
        // Get the entire frame's length.
        //
        uint8_t frameLength = 0;
        if (encrypt)
        {
            frameLength = FCP_Get_Frame_Length((char*)callsign, optDataLength, password);
        }
        else
        {
            frameLength = FCP_Get_Frame_Length((char*)callsign, optDataLength);
        }

        //
        // Create and encoded (at once) frame.
        //
        uint8_t* frame = new uint8_t[frameLength];
        if (encrypt)
        {
            FCP_Encode(frame, (char*)callsign, functionId, optDataLength, (uint8_t*)optData, key, password);
        }
        else
        {
            FCP_Encode(frame, (char*)callsign, functionId, optDataLength);
        }


        //
        // Send the frame wrapped in the gcs protocol.
        //
        char controlByte = directionBit | operationId;
        GroundStationSerialMessage* msg = new GroundStationSerialMessage(controlByte, (char)frameLength, (char*)frame); // the payload IS the FCP Frame above.

        delete[] frame;

        return dynamic_cast<IGroundStationSerialMessage*>(msg);
    }


    void Interpret_Received_Message(IGroundStationSerialMessage* inMsg)
    {
        // route via the operation id
        int operationId = inMsg->GetOperationID();

        if (operationId == 0)
        {
            // handshake
            Interpret_Handshake(inMsg);
        }
        else if (operationId == 1)
        {
            // FCP frame.
            Interpret_FCP_Frame(inMsg);
        }
        else if (operationId == 2)
        {
            // configuration change.
            Interpret_Config_Change(inMsg);
        }
    }

    void Interpret_Handshake(IGroundStationSerialMessage* inMsg)
    {
        GroundStationSerialMessage* msg = dynamic_cast<GroundStationSerialMessage*>(inMsg);

    }

    void Interpret_Config_Change(IGroundStationSerialMessage* inMsg)
    {
        GroundStationSerialMessage* msg = dynamic_cast<GroundStationSerialMessage*>(inMsg);

    }

    void Interpret_FCP_Frame(IGroundStationSerialMessage* inMsg)
    {
        GroundStationSerialMessage* msg = dynamic_cast<GroundStationSerialMessage*>(inMsg);

        // raw characters
        char* payloadData = msg->GetPayload();
        char payloadLength = msg->GetPayloadLengthByte();

        // converted to uint8_t
        uint8_t frameLength = (uint8_t)payloadLength;
        uint8_t* frame = new uint8_t[frameLength];
        for (int i = 0; i < frameLength; i++)
        {
            frame[i] = (uint8_t)(payloadData[i]);
        }



        char * callsign = (char*)m_settings->GetCallsign().c_str(); /// @todo fix stripping of const.

        // get the function ID.
        int16_t functionId = FCP_Get_FunctionID(callsign, frame, frameLength);

        // get the optional data length.
        int16_t optionalDataLength = FCP_Get_OptData_Length(callsign, frame, frameLength, NULL, NULL);

        if (optionalDataLength >= 0)
        {
            // frame contains metadata
            uint8_t* optionalData = new uint8_t[optionalDataLength];
            FCP_Get_OptData(callsign, frame, frameLength, optionalData, NULL, NULL);

            ///////////////////////////////
            /// Interpret the commands. ///
            ///////////////////////////////

            // Public responses
            if (functionId == RESP_PONG)
            {
                // do nothing, already logged.
            }
            else if (functionId == RESP_REPEATED_MESSAGE)
            {

            }
            else if (functionId == RESP_REPEATED_MESSAGE_CUSTOM)
            {

            }
            else if (functionId == RESP_SYSTEM_INFO)
            {

            }
            else if (functionId == RESP_PACKET_INFO)
            {

            }
            else if (functionId == RESP_STATISTICS)
            {

            }
            else if (functionId == RESP_FULL_SYSTEM_INFO)
            {

            }
            else if (functionId == RESP_STORE_AND_FORWARD_ASSIGNED_SLOT)
            {

            }
            else if (functionId == RESP_FORWARDED_MESSAGE)
            {

            }
            else if (functionId == RESP_PUBLIC_PICTURE)
            {

            }
            else if (functionId == RESP_DEPLOYMENT_STATE)
            {

            }
            else if (functionId == RESP_RECORDED_SOLAR_CELLS)
            {

            }
            else if (functionId == RESP_CAMERA_STATE)
            {

            }
            else if (functionId == RESP_RECORDED_IMU)
            {

            }
            else if (functionId == RESP_MANUAL_ACS_RESULT)
            {

            }
            else if (functionId == RESP_GPS_LOG)
            {

            }
            else if (functionId == RESP_GPS_LOG_STATE)
            {

            }
            else if (functionId == RESP_FLASH_CONTENTS)
            {

            }
            else if (functionId == RESP_CAMERA_PICTURE)
            {

            }
            else if (functionId == RESP_CAMERA_PICTURE_LENGTH)
            {

            }
            else if (functionId == RESP_GPS_COMMAND_RESPONSE)
            {

            }
            else if (functionId == RESP_ACKNOWLEDGE)
            {

            }

            // Private
            // there are no privately downlinked messages.


            delete[] optionalData;
        }

        delete[] frame;
    }


    //////
    /// public unencrypted uplink message commands.
    /////
    IGroundStationSerialMessage* Create_CMD_Ping()
    {
        return this->Create_GroundStationSerialMessage(FCPI_FRAME_OP, CMD_PING, 0, nullptr);
    };
    IGroundStationSerialMessage* Create_CMD_Retransmit()
    {
    };
    IGroundStationSerialMessage* Create_CMD_Retransmit_Custom() {};
    IGroundStationSerialMessage* Create_CMD_Transmit_System_Info() {};
    IGroundStationSerialMessage* Create_CMD_Get_Packet_Info() {};
    IGroundStationSerialMessage* Create_CMD_Get_Statistics() {};
    IGroundStationSerialMessage* Create_CMD_Get_Full_System_Info() {};
    IGroundStationSerialMessage* Create_CMD_Store_And_Forward_Add() {};
    IGroundStationSerialMessage* Create_CMD_Store_And_Forward_Request() {};
    IGroundStationSerialMessage* Create_CMD_Request_Public_Picture() {};

    /////
    /// Private commands (encrypted uplink messages)
    /////
    IGroundStationSerialMessage* Create_CMD_Deploy()
    {
        IGroundStationSerialMessage* msg = this->Create_GroundStationSerialMessage(FCPI_FRAME_OP, CMD_DEPLOY, 0, nullptr);

        return msg;
    };
    IGroundStationSerialMessage* Create_CMD_Restart() {};
    IGroundStationSerialMessage* Create_CMD_Wipe_EEPROM() {};
    IGroundStationSerialMessage* Create_CMD_Set_Transmit_Enable() {};
    IGroundStationSerialMessage* Create_CMD_Set_Callsign() {};
    IGroundStationSerialMessage* Create_CMD_Set_SF_Mode() {};
    IGroundStationSerialMessage* Create_CMD_Set_MPPT_Mode() {};
    IGroundStationSerialMessage* Create_CMD_Set_Low_Power_Mode_Enable() {};
    IGroundStationSerialMessage* Create_CMD_Set_Receive_Windows() {};
    IGroundStationSerialMessage* Create_CMD_Record_Solar_Cells() {};
    IGroundStationSerialMessage* Create_CMD_Camera_Capture() {};
    IGroundStationSerialMessage* Create_CMD_Set_Power_Limits() {};
    IGroundStationSerialMessage* Create_CMD_Set_RTC() {};
    IGroundStationSerialMessage* Create_CMD_Record_IMU() {};
    IGroundStationSerialMessage* Create_CMD_Run_Manual_ACS() {};
    IGroundStationSerialMessage* Create_CMD_Log_GPS() {};
    IGroundStationSerialMessage* Create_CMD_Get_GPS_Log() {};
    IGroundStationSerialMessage* Create_CMD_Get_Flash_Contents() {};
    IGroundStationSerialMessage* Create_CMD_Get_Picture_Length() {};
    IGroundStationSerialMessage* Create_CMD_Get_Picture_Burst() {};
    IGroundStationSerialMessage* Create_CMD_Route() {};
    IGroundStationSerialMessage* Create_CMD_Set_Flash_Contents() {};
    IGroundStationSerialMessage* Create_CMD_Set_TLE() {};
    IGroundStationSerialMessage* Create_CMD_Get_GPS_Log_State() {};
    IGroundStationSerialMessage* Create_CMD_Run_GPS_Command() {};
    IGroundStationSerialMessage* Create_CMD_Set_Sleep_Intervals() {};
    IGroundStationSerialMessage* Create_CMD_Abort() {};
    IGroundStationSerialMessage* Create_CMD_Maneuver() {};
    IGroundStationSerialMessage* Create_CMD_Set_ADCS_Parameters() {};
    IGroundStationSerialMessage* Create_CMD_Erase_Flash() {};
private:
    Settings* m_settings = nullptr;
};

#endif // INTERPRETER_H
