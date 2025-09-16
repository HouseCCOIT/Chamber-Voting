/* This program serves as both a test against known hardware (the SNAP PAC
 * Learning Center: http://www.opto22.com/site/pr_details.aspx?cid=1&item=SNAP-PACLC)
 * and as a guide to using the OptoMMP Toolkit to do useful work.
 */

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "O22SIOMM.h"

static void SafelyWaitForEnter()
// Safely wait for the user to press enter, snarfing all input up until that point.
{
    static char buffer[512];

    printf("Press Enter or Return to continue.\n");    
    fgets(buffer, 512, stdin);
}

static void SetDigitalPointState(O22SnapIoMemMap &brain, int point, int state, const char *name)
// This function is simply here to wrap up the SetDigPtState method call and make
// the tour a bit easier to read through.
{
    printf("Setting digital output point %d (%s) to %s\n", point, name, state ? "ON" : "OFF");
    int result = brain.SetDigPtState(point, state);
    if (result != SIOMM_OK) {
        fprintf(stderr, "Error %d setting point %d to %s\n", result, point, state ? "ON" : "OFF");
    }
}

static float CelciusToFahrenheit(float degreesCelcius)
// The temperature input module included with the Learning Center
// (SNAP-AICTD: http://www.opto22.com/site/pr_details.aspx?cid=1&item=SNAP-AICTD)
// reports temperatures back in degrees Celcius, so this function is included to 
// allow us to report in degrees Fahrenheit as well.
{
    return (degreesCelcius * 1.8f) + 32.0f;
}

static void PlayWithFuelDisplay(O22SnapIoMemMap &brain)
// This function will mirror the current setting of the Fuel Level dial to the Fuel Display
// window for 10 seconds. It's wrapped into a function simply because it happens twice in the
// tour.
{
    time_t startTime, currentTime;
    startTime = time(NULL);
    do {
        float fuelLevel;
        int result = brain.GetAnalogPointValueEx(4, 0, &fuelLevel);
        if (result != SIOMM_OK) {
            fprintf(stderr, "Error %d when trying to read the fuel level dial.\n", result);
            break;
        }

        result = brain.SetAnalogPointValueEx(2, 0, fuelLevel);
        if (result != SIOMM_OK) {
            fprintf(stderr, "Error %d when trying to set the fuel level display.\n", result);
            break;
        }

#ifdef _WIN32
        Sleep(100);
#else
        usleep(100);
#endif
        currentTime = time(NULL);
    } while((currentTime - startTime) < 10);
}

int main(int argc, char **argv)
{
    char *address = NULL;
    int returnValue = 0;

    if (argc < 2) {
        printf("Usage: learningcenter <address>\n");
        return 0;
    }

    address = argv[1];

    printf("Connecting to the Ethernet Brain at address %s", address);
    fflush(stdout);

    O22SnapIoMemMap brain;
    int result;

    // We first call OpenEnet2 to connect to the SNAP-PAC-R1 controller in the Learning
    // Center. It will need to have an IP address associated with it first; you'll need
    // to use PAC Manager to do that.
    //
    // The arguments to OpenEnet2 are:
    //  + ip address or hostname (a string)
    //  + port number (an integer, normally 2001)
    //  + a timeout value (an integer, 10 seconds here)
    //  + whether to clear the power-up flag on connection (an integer, treated as a boolean)
    //  + whether to use TCP or UDP for the connection (SIOMM_TCP or SIOMM_UDP)
    if ((result = brain.OpenEnet2(address, 2001, 10000, 1, SIOMM_TCP)) != SIOMM_OK) {
        fprintf(stderr, "\nError %d connecting to the brain\n", result);
        returnValue = 1;
        goto cleanup;
    }

    // The OptoMMP Toolkit uses non-blocking sockets, so OpenEnet2 can return before the connection
    // is actually established. Here we wait in a busy loop until we're sure the brain is ready to use.
    do {
        printf(".");
        fflush(stdout);
        result = brain.IsOpenDone();
    } while (result == SIOMM_ERROR_NOT_CONNECTED_YET);

    if (result == SIOMM_OK) {
        printf("Done.\n");
    } else {
        fprintf(stderr, "\nError %d while connecting to the brain\n", result);
        returnValue = 1;
        goto cleanup;
    }

    // Digital inputs, high density digital modules, and analog modules are all recognized
    // automatically when a controller boots up. Digital outputs are not, and so need to be
    // configured before you can use them.
    //
    // If you're loading a strategy onto a controller via PAC Manager, you've already done this.
    // For the sake of this test and example, we're going to assume that the controller is booting
    // up fresh and not running a strategy, so we need to manually configure the digital output
    // module (SNAP-ODC5SRC: http://www.opto22.com/site/pr_details.aspx?cid=1&item=SNAP-ODC5SRC)
    // before continuing.
    //
    // When put together according to the instructions in the Learning Center User's Guide, the
    // digital output module will be in slot 1, which means that it corresponds to the second group
    // of digital points, points 4 through 7.
    //
    // O22SnapIoMemMap::ConfigurePoint takes two arguments: the point number in question, and the
    // point type value. A digital output is type 0x180 (or 384 in decimal), an input is 0x100 (256).

    printf("\nConfiguring points 4 through 7 as digital output points...\n");
    brain.ConfigurePoint(4, 0x180);
    brain.ConfigurePoint(5, 0x180);
    brain.ConfigurePoint(6, 0x180);
    brain.ConfigurePoint(7, 0x180);
    printf("Done, moving on to reading digital inputs.\n");

    SafelyWaitForEnter();

    // Lets try reading the digital inputs. Set up normally, there is a 4 channel digital input
    // module (SNAP-IDC5D: http://www.opto22.com/site/pr_details.aspx?cid=1&item=SNAP-IDC5D) in
    // slot 0, points 0 through 3. Those points are hooked up to the following inputs:
    //  + Point 0 - The Emergency switch
    //  + Point 1 - The POS switch
    //  + Point 2 - The Freezer Door switch
    //  + Point 3 - The Photo Sensor switch
    printf("Going to read the digital input points. Feel free to toggle them as you like\n");
    printf("and I will report the switches you have set back to you.\n");
    SafelyWaitForEnter();
    for (int point = 0;point < 4; point++) {
        int state;
        result = brain.GetDigPtState(point, &state);
        printf("  Point %d is %s\n", point, state ? "ON" : "OFF");
    }

    printf("\nDone with the digital inputs, let's move on to the outputs. Let me know when\n");
    printf("you're ready.\n");
    SafelyWaitForEnter();

    // Now let's try setting some values on the digital output. Again, the digital output module is
    // in slot 1, corresponding to points 4 through 7. Those points are hooked up to the following:
    //  + Point 4 - Alarm
    //  + Point 5 - Freezer Door Status
    //  + Point 6 - Inside Light
    //  + Point 7 - Outside Light
    //
    // In the interest of not annoying people who may be nearby, we'll leave point 7 alone, but feel
    // free to change things if you want.
    //
    // NOTE: The actual method calls are wrapped up in the function SetDigitalPointState, defined
    // up at the top of this file. I figured it was easier to read that way.
    SetDigitalPointState(brain, 4, 0, "Alarm");
    SetDigitalPointState(brain, 5, 1, "Freezer Door Status");
    SetDigitalPointState(brain, 6, 1, "Inside Light");
    SetDigitalPointState(brain, 7, 0, "Outside Light");

    printf("\nAt this point, assuming there were no errors, the Inside Light and Freezer Door Status\n");
    printf("LEDs should be lit up, the Outside Light LED should be off, and you shouldn't be hearing\n");
    printf("an annoying beep.\n");

    SafelyWaitForEnter();

    printf("Now let's take a look at the analog I/O modules.\n");
    SafelyWaitForEnter();

    // In slot 3 there is an analog temperature input module (SNAP-AICTD:
    // http://www.opto22.com/site/pr_details.aspx?cid=1&item=SNAP-AICTD).
    //
    // New in firmware 8.0, we can address analog points in an expanded memory map that allows
    // for much greater point density (up to 64 points) and more modules in a rack (up to 64).
    // The new method O22SnapIoMemMap::GetAnalogPointValueEx allows us to use that expanded 
    // memory map and lets us specify the module and point directly, instead of having to do the
    // math to figure out the proper point number as we did for the digital points.
    //
    // Unfortunately, digital points haven't gotten the same treatment. But anyway, let's 
    // find out what the temperature in here is.
    float degreesCelcius;
    result = brain.GetAnalogPointValueEx(3, 0, &degreesCelcius);
    if (result != SIOMM_OK) {
        fprintf(stderr, "Error %d when trying to read the temperature probe in module 3, point 0\n", result);
    }

    printf("It is currently %.2f degrees Celcius (%.2f degrees Fahrenheit) in here.\n",
            degreesCelcius, CelciusToFahrenheit(degreesCelcius));

    float minimumTemperature, maximumTemperature;
    result = brain.GetAnalogPointMinValueEx(3, 0, &minimumTemperature);
    if (result != SIOMM_OK) {
        fprintf(stderr, "Error %d when trying to read the minimum value from module 3, point 0\n", result);
    }

    result = brain.GetAnalogPointMaxValueEx(3, 0, &maximumTemperature);
    if (result != SIOMM_OK) {
        fprintf(stderr, "Error %d when trying to read the maximum value from module 3, point 0\n", result);
    }

    printf("The minimum temperature reported by the probe was %.2f degrees Celcius (%.2f degrees Fahrenheit).\n",
            minimumTemperature, CelciusToFahrenheit(minimumTemperature));

    printf("The maximum temperature reported by the probe was %.2f degrees Celcius (%.2f degrees Fahrenheit).\n",
            maximumTemperature, CelciusToFahrenheit(maximumTemperature));

    SafelyWaitForEnter();

    // Next, let's play around with an analog input and output you can control. We'll read the current value
    // of the Fuel Level dial and mirror its value to the Fuel Display window.
    
    printf("Now let's play around with an analog input and output you can control. For the next 10 seconds\n");
    printf("(after you hit enter) turning the Fuel Level dial will attempt to mirror it's value to the\n");
    printf("Fuel Display window.\n");
    SafelyWaitForEnter();

    // This function is defined up at the top of the file. We'll be doing the same thing again in a moment,
    // so it's wrapped into a function to make this tour easier to follow.
    PlayWithFuelDisplay(brain);

    // Depending on what's happened with your Learning Center recently, you may have seen that the Fuel
    // Display window is only showing half the range of the Fuel Level dial. (e.g. the needle only won't
    // go above 5). 
    //
    // Both the input (SNAP-AIV: http://www.opto22.com/site/pr_details.aspx?cid=1&item=SNAP-AIV) and 
    // output (SNAP-OIV-27: http://www.opto22.com/site/pr_details.aspx?cid=1&item=SNAP-AOV-27) modules
    // have a range of -10 to 10 VDC, but the potentiometer use in the Fuel Level dial only puts out
    // 0 to 5 VDC. To account for that, we have two options:
    //  1 - Just double the value that gets read from the Fuel Level dial
    //  2 - Use the input module's scaling feature to fix the input for us.
    //
    // If you've worked through the Learning Center User's Guide, you did this on page 66. Basically,
    // we want 5 VDC on the potentiometer to be represented as the value 10. To do that, we want to
    // tell the point to scale values from -20 to 20, instead of the normal -10 to 10.

    printf("Setting scale values on the Fuel Level input, and re-running the test.\n");
    result = brain.SetPointScale(4, 0, 20.0f, -20.0f);
    if (result != SIOMM_OK) {
        fprintf(stderr, "Error %d when trying to set scaling values on module 4, point 0.\n", result);
    }
    SafelyWaitForEnter();

    // Now, let's try that again.
    PlayWithFuelDisplay(brain);

    printf("Done. Resetting the scale on the Fuel Level input so as not to confuse folks later.\n");
    result = brain.SetPointScale(4, 0, 10.0f, -10.0f);
    if (result != SIOMM_OK) {
        fprintf(stderr, "Error %d when trying to set scaling values on module 4, point 0.\n", result);
    }

    // And at this point we're done with the tour.
cleanup:
    printf("Tour complete.\n");
    brain.Close();
    return returnValue;
}


