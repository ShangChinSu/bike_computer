#include <Arduino.h>
#include <Arduino_GFX_Library.h>
#include <WiFi.h>

#include "system_config.h"
#include "display.h"
#include "app_wifi.h"
#include "metrics.h"
#include "events.h"
#include "drivers/TouchDriver.h"
#include "sensors/ImuSensor.h"
#include "ui/ScreenManager.h"
#include "ui/DashboardScreen.h"
#include "ui/TripStatsScreen.h"
#include "ui/ClockScreen.h"


// * Hardware Display Setup
Arduino_DataBus *bus = new Arduino_ESP32SPI(TFT_DC, TFT_CS, TFT_SCK, TFT_MOSI, TFT_MISO);
Arduino_GFX *gfx = new Arduino_GC9A01(bus, TFT_RST, 0, true);

// * Touch & Sensor Subsystem
TouchDriver touch;
ImuSensor imu;

// * UI Subsystem
ScreenManager screen_manager(gfx);
DashboardScreen dashboard_screen;
TripStatsScreen trip_stats_screen;
ClockScreen clock_screen;

// * Telemetry State
RideMetrics ride_metrics;

// * Timing
uint32_t last_ui_tick = 0;
uint32_t last_sim_tick = 0;


/**
 * @brief Scans the shared I2C bus (SDA: 4, SCL: 5) to identify all connected chips.
 */
void scanI2CBus()
{
	Serial.println("[I2C] Scanning bus (SDA: 4, SCL: 5)...");
	uint8_t count = 0;

	for (uint8_t addr = 1; addr < 127; ++addr)
	{
		Wire.beginTransmission(addr);
		if (Wire.endTransmission() == 0)
		{
			Serial.printf("[I2C] Found device at 0x%02X", addr);
			if (addr == 0x15) Serial.print(" -> CST816 Touch Controller");
			else if (addr == 0x68) Serial.print(" -> MPU6050 / MPU6500 IMU");
			else if (addr == 0x6B) Serial.print(" -> QMI8658 / LSM6DS IMU");
			else if (addr == 0x76 || addr == 0x77) Serial.print(" -> BMP280 / BME280 Barometer");
			Serial.println();
			count++;
		}
	}

	if (count == 0)
	{
		Serial.println("[I2C] No devices detected.");
	}
	else
	{
		Serial.printf("[I2C] Scan complete. %u device(s) found.\n", count);
	}
}


void setup()
{
	Serial.begin(115200);
	delay(500);
	Serial.println("\n=== Bike Computer Starting ===");

	// 1. Initialize Display Backlight & Controller
	pinMode(TFT_BL, OUTPUT);
	digitalWrite(TFT_BL, HIGH);

	if (!gfx->begin())
	{
		Serial.println("[Display] Initialization failed!");
	}
	else
	{
		Serial.println("[Display] GC9A01 Ready.");
	}

	// 2. Initialize Touch Controller (CST816D)
	if (touch.begin(TOUCH_SDA, TOUCH_SCL, TOUCH_RST, TOUCH_INT))
	{
		Serial.println("[Touch] CST816D Ready.");
	}
	else
	{
		Serial.println("[Touch] CST816D not responding or busy.");
	}

	// 3. Scan I2C bus and probe IMU
	scanI2CBus();
	if (imu.begin())
	{
		Serial.printf("[IMU] %s detected & initialized!\n", imu.getTypeName());
	}
	else
	{
		Serial.println("[IMU] No IMU hardware detected on I2C bus.");
	}

	// 4. Register UI Screens
	screen_manager.addScreen(&dashboard_screen);
	screen_manager.addScreen(&trip_stats_screen);
	screen_manager.addScreen(&clock_screen);
	Serial.printf("[UI] %u screens loaded. Ready for swipe gestures.\n", screen_manager.getScreenCount());

	// 4. Initial Sample Telemetry
	ride_metrics.speed_kph = 25.4f;
	ride_metrics.cadence_rpm = 82;
	ride_metrics.heart_rate_bpm = 138;
	ride_metrics.avg_speed_kph = 22.1f;
	ride_metrics.max_speed_kph = 37.8f;
	ride_metrics.trip_distance_km = 4.25f;
	ride_metrics.moving_time_sec = 690; // 11m 30s
	ride_metrics.battery_percent = 92;
	ride_metrics.is_recording = true;

	// 5. Connect WiFi & sync time (non-blocking timeout)
	wifi::initialization();
	configTime(system_config::gmtOffset_sec, system_config::daylightOffset_sec, system_config::ntpServer1, system_config::ntpServer2);
}


void loop()
{
	uint32_t now = millis();

	// 1. Poll Touch & Gestures
	if (touch.update())
	{
		TouchGesture gesture = touch.getGesture();
		if (gesture != TouchGesture::NONE)
		{
			Serial.printf("[Touch] Gesture: %s\n", TouchDriver::gestureToString(gesture));
			screen_manager.processGesture(gesture);
		}
	}

	// 2. Poll IMU sensor if available (10 Hz update)
	if (imu.isConnected())
	{
		imu.update();
		ride_metrics.grade_percent = imu.getGradePercent();
		ride_metrics.pitch_deg     = imu.getPitchDeg();
	}

	// 3. Telemetry update (1 Hz simulation to showcase live dashboard)
	if (now - last_sim_tick >= 1000)
	{
		last_sim_tick = now;

		if (ride_metrics.is_recording)
		{
			ride_metrics.moving_time_sec++;
			ride_metrics.trip_distance_km += (ride_metrics.speed_kph / 3600.0f);

			// Gentle speed oscillation between 23.0 and 27.5 km/h for visual feedback
			static float phase = 0.0f;
			phase += 0.1f;
			ride_metrics.speed_kph = 25.0f + 2.5f * sinf(phase);
		}
	}

	// 3. UI Render (every 60ms ~ 16 FPS for smooth updates without flickering)
	if (now - last_ui_tick >= 60)
	{
		last_ui_tick = now;
		screen_manager.render(ride_metrics);
	}
}
