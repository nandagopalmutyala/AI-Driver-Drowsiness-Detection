import cv2
import mediapipe as mp
import numpy as np
import serial
import time

# ---------- Arduino Connection ----------
arduino = serial.Serial('COM3', 9600)
time.sleep(2)

# ---------- MediaPipe Setup ----------
mp_face_mesh = mp.solutions.face_mesh
face_mesh = mp_face_mesh.FaceMesh()

# ---------- Facial Landmarks ----------
LEFT_EYE = [33, 160, 158, 133, 153, 144]
RIGHT_EYE = [362, 385, 387, 263, 373, 380]

MOUTH = [61, 81, 13, 311, 291, 178]

NOSE = 1
CHIN = 152

# ---------- EAR Function ----------
def EAR(eye):
    A = np.linalg.norm(eye[1] - eye[5])
    B = np.linalg.norm(eye[2] - eye[4])
    C = np.linalg.norm(eye[0] - eye[3])

    ear = (A + B) / (2.0 * C)
    return ear

# ---------- MAR Function ----------
def MAR(mouth):
    A = np.linalg.norm(mouth[1] - mouth[5])
    B = np.linalg.norm(mouth[2] - mouth[4])
    C = np.linalg.norm(mouth[0] - mouth[3])

    mar = (A + B) / (2.0 * C)
    return mar

# ---------- Camera ----------
cap = cv2.VideoCapture(0)

# ---------- Threshold Values ----------
EAR_THRESHOLD = 0.25
MAR_THRESHOLD = 0.6
BLINK_THRESHOLD = 0.23

CONSEC_FRAMES = 15

eye_counter = 0
mouth_counter = 0
blink_counter = 0
blink_total = 0

motor_state = 1

# ---------- Main Loop ----------
while True:

    ret, frame = cap.read()

    if not ret:
        break

    frame = cv2.flip(frame, 1)

    rgb = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)

    results = face_mesh.process(rgb)

    fatigue = False

    if results.multi_face_landmarks:

        for face_landmarks in results.multi_face_landmarks:

            h, w, _ = frame.shape

            # ---------- LEFT EYE ----------
            left_eye = []

            for idx in LEFT_EYE:

                lm = face_landmarks.landmark[idx]

                x = int(lm.x * w)
                y = int(lm.y * h)

                left_eye.append([x, y])

                cv2.circle(frame, (x, y), 2, (0, 255, 0), -1)

            # ---------- RIGHT EYE ----------
            right_eye = []

            for idx in RIGHT_EYE:

                lm = face_landmarks.landmark[idx]

                x = int(lm.x * w)
                y = int(lm.y * h)

                right_eye.append([x, y])

                cv2.circle(frame, (x, y), 2, (255, 0, 0), -1)

            left_eye = np.array(left_eye)
            right_eye = np.array(right_eye)

            # ---------- EAR ----------
            left_ear = EAR(left_eye)
            right_ear = EAR(right_eye)

            ear = (left_ear + right_ear) / 2.0

            # ---------- MOUTH ----------
            mouth = []

            for idx in MOUTH:

                lm = face_landmarks.landmark[idx]

                x = int(lm.x * w)
                y = int(lm.y * h)

                mouth.append([x, y])

                cv2.circle(frame, (x, y), 2, (0, 0, 255), -1)

            mouth = np.array(mouth)

            # ---------- MAR ----------
            mar = MAR(mouth)

            # ---------- HEAD POSE ----------
            nose = face_landmarks.landmark[NOSE]
            chin = face_landmarks.landmark[CHIN]

            nose_y = int(nose.y * h)
            chin_y = int(chin.y * h)

            head_down = chin_y - nose_y

            # ---------- BLINK DETECTION ----------
            if ear < BLINK_THRESHOLD:

                blink_counter += 1

            else:

                if blink_counter > 2:
                    blink_total += 1

                blink_counter = 0

            # ---------- EYE DROWSINESS ----------
            if ear < EAR_THRESHOLD:

                eye_counter += 1

            else:

                eye_counter = 0

            # ---------- YAWNING ----------
            if mar > MAR_THRESHOLD:

                mouth_counter += 1

            else:

                mouth_counter = 0

            # ---------- DISPLAY ----------
            cv2.putText(frame,
                        f"EAR: {ear:.2f}",
                        (20, 40),
                        cv2.FONT_HERSHEY_SIMPLEX,
                        0.7,
                        (255, 255, 0),
                        2)

            cv2.putText(frame,
                        f"MAR: {mar:.2f}",
                        (20, 70),
                        cv2.FONT_HERSHEY_SIMPLEX,
                        0.7,
                        (0, 255, 255),
                        2)

            cv2.putText(frame,
                        f"Blinks: {blink_total}",
                        (20, 100),
                        cv2.FONT_HERSHEY_SIMPLEX,
                        0.7,
                        (255, 255, 255),
                        2)

            # ---------- FATIGUE CONDITIONS ----------
            if eye_counter >= CONSEC_FRAMES:

                fatigue = True

                cv2.putText(frame,
                            "EYES CLOSED",
                            (200, 40),
                            cv2.FONT_HERSHEY_SIMPLEX,
                            0.8,
                            (0, 0, 255),
                            2)

            if mouth_counter >= CONSEC_FRAMES:

                fatigue = True

                cv2.putText(frame,
                            "YAWNING",
                            (200, 70),
                            cv2.FONT_HERSHEY_SIMPLEX,
                            0.8,
                            (0, 0, 255),
                            2)

            if head_down > 120:

                fatigue = True

                cv2.putText(frame,
                            "HEAD DOWN",
                            (200, 100),
                            cv2.FONT_HERSHEY_SIMPLEX,
                            0.8,
                            (0, 0, 255),
                            2)

    # ---------- MOTOR CONTROL ----------
    if fatigue:

        if motor_state != 0:

            arduino.write(b'0')

            motor_state = 0

        cv2.putText(frame,
                    "DROWSY DRIVER ALERT",
                    (80, 150),
                    cv2.FONT_HERSHEY_SIMPLEX,
                    1,
                    (0, 0, 255),
                    3)

    else:

        if motor_state != 1:

            arduino.write(b'1')

            motor_state = 1

        cv2.putText(frame,
                    "NORMAL",
                    (250, 150),
                    cv2.FONT_HERSHEY_SIMPLEX,
                    1,
                    (0, 255, 0),
                    2)

    # ---------- SHOW WINDOW ----------
    cv2.imshow("Driver Monitoring System", frame)

    # ---------- EXIT ----------
    if cv2.waitKey(1) & 0xFF == 27:
        break

# ---------- RELEASE ----------
cap.release()

cv2.destroyAllWindows()

arduino.close()