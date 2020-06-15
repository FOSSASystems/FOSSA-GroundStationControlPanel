#ifndef INTERPRETER_H
#define INTERPRETER_H


#include <cstring>
#include <stdexcept>

#include <FOSSA-Comms.h>

#include "Interfaces/IInterpreter.h"

#include "Components/GroundStationSerialMessage.h"
#include "Components/Settings.h"

class Interpreter : public IInterpreter
{
public:
    virtual ~Interpreter() {};

    /// The ground station serial message is an internal command structure.
    /// This creates it from raw serial datagrams.
    /// @todo checks on the serial data length.
    virtual GroundStationSerialMessage* SerialData_To_GroundStationSerialMessage(const char* serialData, char serialDataLength) override final
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

        return message;
    }

    /// this is the base class for all the Create_CMD_XXX methods.
    virtual GroundStationSerialMessage* Create_GroundStationSerialMessage(char directionBit, char operationId, const char * payloadData) override final
    {
        /// @todo check for over-run.
        char length = strlen(payloadData);

        char* payload = new char[length];
        memcpy(payload, payloadData, length);

        char controlByte = directionBit | operationId;

        GroundStationSerialMessage* datagram = new GroundStationSerialMessage(controlByte, length, payload);

        delete[] payload;

        return datagram;
    }


    void Interpret_Received_Message(IGroundStationSerialMessage* inMsg, Settings* settings)
    {
        GroundStationSerialMessage* msg = dynamic_cast<GroundStationSerialMessage*>(inMsg);

        // route via the operation id
        int operationId = msg->GetOperationID();

        if (operationId == 0)
        {
            // handshake
            Interpret_Handshake(inMsg, settings);
        }
        else if (operationId == 1)
        {
            // FCP frame.
            Interpret_FCP_Frame(inMsg, settings);
        }
        else if (operationId == 2)
        {
            // configuration change.
            Interpret_Config_Change(inMsg, settings);
        }
    }

    void Interpret_Handshake(IGroundStationSerialMessage* inMsg, Settings* settings)
    {

    }

    void Interpret_Config_Change(IGroundStationSerialMessage* inMsg, Settings* settings)
    {

    }

    void Interpret_FCP_Frame(IGroundStationSerialMessage* inMsg, Settings* settings)
    {

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



        char * callsign = (char*)settings->GetCallsign().c_str(); /// @todo fix stripping of const.

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
    GroundStationSerialMessage* Create_CMD_Ping() {};
    GroundStationSerialMessage* Create_CMD_Retransmit() {};
    GroundStationSerialMessage* Create_CMD_Retransmit_Custom() {};
    GroundStationSerialMessage* Create_CMD_Transmit_System_Info() {};
    GroundStationSerialMessage* Create_CMD_Get_Packet_Info() {};
    GroundStationSerialMessage* Create_CMD_Get_Statistics() {};
    GroundStationSerialMessage* Create_CMD_Get_Full_System_Info() {};
    GroundStationSerialMessage* Create_CMD_Store_And_Forward_Add() {};
    GroundStationSerialMessage* Create_CMD_Store_And_Forward_Request() {};
    GroundStationSerialMessage* Create_CMD_Request_Public_Picture() {};

    /////
    /// Private commands (encrypted uplink messages)
    /////
    GroundStationSerialMessage* Create_CMD_Deploy() {};
    GroundStationSerialMessage* Create_CMD_Restart() {};
    GroundStationSerialMessage* Create_CMD_Wipe_EEPROM() {};
    GroundStationSerialMessage* Create_CMD_Set_Transmit_Enable() {};
    GroundStationSerialMessage* Create_CMD_Set_Callsign() {};
    GroundStationSerialMessage* Create_CMD_Set_SF_Mode() {};
    GroundStationSerialMessage* Create_CMD_Set_MPPT_Mode() {};
    GroundStationSerialMessage* Create_CMD_Set_Low_Power_Mode_Enable() {};
    GroundStationSerialMessage* Create_CMD_Set_Receive_Windows() {};
    GroundStationSerialMessage* Create_CMD_Record_Solar_Cells() {};
    GroundStationSerialMessage* Create_CMD_Camera_Capture() {};
    GroundStationSerialMessage* Create_CMD_Set_Power_Limits() {};
    GroundStationSerialMessage* Create_CMD_Set_RTC() {};
    GroundStationSerialMessage* Create_CMD_Record_IMU() {};
    GroundStationSerialMessage* Create_CMD_Run_Manual_ACS() {};
    GroundStationSerialMessage* Create_CMD_Log_GPS() {};
    GroundStationSerialMessage* Create_CMD_Get_GPS_Log() {};
    GroundStationSerialMessage* Create_CMD_Get_Flash_Contents() {};
    GroundStationSerialMessage* Create_CMD_Get_Picture_Length() {};
    GroundStationSerialMessage* Create_CMD_Get_Picture_Burst() {};
    GroundStationSerialMessage* Create_CMD_Route() {};
    GroundStationSerialMessage* Create_CMD_Set_Flash_Contents() {};
    GroundStationSerialMessage* Create_CMD_Set_TLE() {};
    GroundStationSerialMessage* Create_CMD_Get_GPS_Log_State() {};
    GroundStationSerialMessage* Create_CMD_Run_GPS_Command() {};
    GroundStationSerialMessage* Create_CMD_Set_Sleep_Intervals() {};
    GroundStationSerialMessage* Create_CMD_Abort() {};
    GroundStationSerialMessage* Create_CMD_Maneuver() {};
    GroundStationSerialMessage* Create_CMD_Set_ADCS_Parameters() {};
    GroundStationSerialMessage* Create_CMD_Erase_Flash() {};

};

#endif // INTERPRETER_H
