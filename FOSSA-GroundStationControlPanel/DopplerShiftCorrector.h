#ifndef DOPPLERSHIFTCORRECTOR_H
#define DOPPLERSHIFTCORRECTOR_H

#include "3rdparty/fossasattracker/src/SatelliteSimulation.hpp"
#include "3rdparty/fossasattracker/src/TLE.hpp"
#include "3rdparty/fossasattracker/libs/gsat-r11-fixed/src/gObserver.hpp"
#include "3rdparty/fossasattracker/libs/gsat-r11-fixed/src/gTime.hpp"

class DopplerShiftCorrector
{
public:
    DopplerShiftCorrector()
    {
    }

    ~DopplerShiftCorrector()
    {

    }

    bool GetDopplerShiftNow(std::string satelliteName, double* destDopplerShift)
    {
        if (m_simulation != nullptr)
        {
            delete m_simulation;
        }
        if (m_tle == nullptr)
        {
            return false;
        }

        gTime time = gTime::getCurrentTime();

        m_simulation = new FOSSASatTracker::SatelliteSimulation(satelliteName.c_str(), *m_tle);
        FOSSASatTracker::SimulationResult result = m_simulation->Simulate(m_observer, time);

        //        if (result.HasSatelliteInformation())
        //        {
        //            FOSSASatTracker::SatelliteInformation satelliteInformation = result.GetSatelliteInformation();
        //        }
        if (result.HasObserverInformation())
        {
            FOSSASatTracker::ObserverInformation observerInformation = result.GetObserverInformation();
            double dopplerShift = observerInformation.GetDopplerShift();
            (*destDopplerShift) = dopplerShift;
            return true;
        }

        return false;
    }

    void SetObserverParameters(double latitude, double longitude, double attitude)
    {
        m_observer.setPosition(latitude, longitude, attitude);
    }

    void SetTLELines(std::string line1, std::string line2)
    {
        if (m_tle == nullptr)
        {
            delete m_tle;
        }

        if (line1.length() <= 0)
        {
            return;
        }
        if (line2.length() <= 0)
        {
            return;
        }

        char line1str[70];
        char line2str[70];
        for (int i = 0; i < 70; i++)
        {
            line1str[i] = line1[i];
            line2str[i] = line2[i];
        }

        m_tle = new FOSSASatTracker::TLE(line1str, line2str);
    }
private:
    FOSSASatTracker::SatelliteSimulation* m_simulation = nullptr;
    FOSSASatTracker::TLE* m_tle = nullptr;
    gObserver m_observer;
};

#endif // DOPPLERSHIFTCORRECTOR_H
