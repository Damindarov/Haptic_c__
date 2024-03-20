#include <iostream>

#include <HD/hd.h>
#include <HDU/hduError.h>
#include <HDU/hduVector.h>

HDCallbackCode HDCALLBACK positionCallback(void *userData)
{
    hdBeginFrame(hdGetCurrentDevice());

    hduVector3Dd position;
    hdGetDoublev(HD_CURRENT_POSITION, position);

    hdEndFrame(hdGetCurrentDevice());

    std::cout << "Current position: ("
              << position[0] << ", "
              << position[1] << ", "
              << position[2] << ")"
              << std::endl;

    return HD_CALLBACK_CONTINUE;
}

HDCallbackCode HDCALLBACK forceCallback(void *userData)
{
    hdBeginFrame(hdGetCurrentDevice());

    // Текущее положение стилуса
    hduVector3Dd position;
    hdGetDoublev(HD_CURRENT_POSITION, position);

    // Вычисляем силу как функцию от положения. В этом случае просто
    // притягиваем стилус к началу координат.
    // Пример: простая пропорциональная сила, направленная к центру.
    hduVector3Dd force;
    std::cout << "Current position: ("
              << position[0] << ", "
              << position[1] << ", "
              << position[2] << ")"
              << std::endl;
    force[0] = -position[0] * 0.11; // Коэффициент управляет "жесткостью" силы
    force[1] = -position[1] * 0.11;
    force[2] = -position[2] * 0.11;

    hdSetDoublev(HD_CURRENT_FORCE, force);

    hdEndFrame(hdGetCurrentDevice());

    return HD_CALLBACK_CONTINUE;
}

int main()
{
    HDErrorInfo error;
    HHD hHD = hdInitDevice(HD_DEFAULT_DEVICE);
    if (HD_DEVICE_ERROR(error = hdGetError()))
    {
        std::cerr << "Failed to initialize haptic device: " << hdGetErrorString(error.errorCode) << std::endl;
        return -1;
    }

    hdEnable(HD_FORCE_OUTPUT);
    hdStartScheduler();
    if (HD_DEVICE_ERROR(error = hdGetError()))
    {
        std::cerr << "Failed to start the scheduler: " << hdGetErrorString(error.errorCode) << std::endl;
        hdDisableDevice(hHD);
        return -1;
    }

//    HDulong callbackHandle = hdScheduleAsynchronous(positionCallback, NULL, HD_DEFAULT_SCHEDULER_PRIORITY);
    HDulong callbackHandle = hdScheduleAsynchronous(forceCallback, NULL, HD_DEFAULT_SCHEDULER_PRIORITY);

    std::cout << "Press Enter to quit..." << std::endl;
    std::cin.get();

    hdUnschedule(callbackHandle);
    hdStopScheduler();
    hdDisableDevice(hHD);

    return 0;
}