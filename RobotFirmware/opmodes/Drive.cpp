#include "NRL.h"

// Declare your hardware here (file-scope), e.g.:
// static HexaDCMotor leftMotor {{ .dirPin = MOTOR_L_DIR, .pwmPin = MOTOR_L_PWM }};
static HexaDCMotor leftMotor{{ .dirPin = MOTOR_L_DIR, .pwmPin = MOTOR_L_PWM, .flipped = true }};
static HexaDCMotor rightMotor{{ .dirPin = MOTOR_R_DIR, .pwmPin = MOTOR_R_PWM }};
static TankDrive drive(leftMotor, rightMotor);

float maxArm = 35.0f;
float minArm = 90.0f;
static HexaServo arm{{ .signalPin = SERVO_1, .startAngle = maxArm, .settleMs = 0 }};

float maxGrip = 5.0f;
float minGrip = 65.0f;
static HexaServo grip{{ .signalPin = SERVO_2, .startAngle = maxGrip, .settleMs = 250 }};

float fspeed = 1.0f; // fast mode
float mspeed = 0.5f; // medium mode
float sspeed = 0.25f; // slow mode

class Drive : public NRLOpMode {
public:
    void init() override {
        // Runs once when INIT is pressed — begin() your hardware here.
        //Motor Init
        leftMotor.begin();
        rightMotor.begin();

        //Arm and gripper Init
        arm.begin();
        grip.begin();
        
        //Basic arm and gripper control
        gamepad1.onPress(BTN_DPAD_UP, [] { arm.setPosition(maxArm); });
        gamepad1.onPress(BTN_DPAD_DOWN, [] { arm.setPosition(minArm); });
        gamepad1.onPress(BTN_DPAD_LEFT, [] { grip.setPosition(minGrip); });
        gamepad1.onPress(BTN_DPAD_RIGHT, [] { grip.setPosition(maxGrip); });
        
        //Speed modes
        gamepad1.onPress(BTN_X, [] { drive.setScale(fspeed); });
        gamepad1.onPress(BTN_Y, [] { drive.setScale(mspeed); });
        gamepad1.onPress(BTN_A, [] { drive.setScale(sspeed); });
    }

    void loop() override {
        // Runs at 100 Hz until STOP — read gamepad1, drive motors here.
        //
        // Control each part in ONE place: loop() or an action, not
        // both. Actions run before loop() each tick, so loop() would
        // overwrite them. To share one part, step aside while a
        // sequence plays:  if (!isActionRunning()) { ... }
        
        //Drive
        drive.drive(gamepad1.leftY(), gamepad1.rightX());

        //Speed Mode Telemetry
        if (drive.getScale() == fspeed) telemetry.addData("Speed", "Fast");
        else if (drive.getScale() == mspeed) telemetry.addData("Speed", "Medium");
        else telemetry.addData("Speed", "Slow");

        //Arm Precision Mode
        if (gamepad1.pressed(BTN_LB)) {
            float joystick = gamepad1.rightY();

            float position = maxArm +
                ((joystick + 1.0f) / 2.0f) * (minArm - maxArm);

            arm.setPosition(position);
        }
    }

    void stop() override {
        // Runs once on STOP — stop your motors/servos here.
        drive.stop();
        arm.detach();
        grip.detach();
    }
};

REGISTER_OPMODE(Drive, "Drive", TELEOP);
