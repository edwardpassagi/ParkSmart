using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class IRSensorScript : MonoBehaviour
{
    private struct LocalMinimum {
        float reading;
        float angle;
    }
    enum SystemState {Dormant, RestoreEvauatorAngle, Awake, Alarming};
    enum TurnDirection {CW, CCW};
    private const float TURN_SPEED = 0.525f;
    private float SENTINEL_ANGLE = 60.0f;
    public int MAX_OSCILLATIONS = 4;
    public GameObject LaserPrefab;
    private LaserMono left_sentinel;
    private LaserMono evaluator;
    private LaserMono right_sentinel;
    private bool areLasersConfigured = false;
    private bool isEvaluatorMoving = true;
    private int oscillations = 0;
    private TurnDirection turnDirection = TurnDirection.CCW;
    private float previousReading = Mathf.Infinity;
    private SystemState systemState = SystemState.Dormant;

    void SetupDormantPhase() {
        left_sentinel.SetActiveState(true);
        evaluator.SetActiveState(false);
        right_sentinel.SetActiveState(true);
        previousReading = Mathf.Infinity;
        oscillations = 0;
    }

    // Start is called before the first frame update
    void Start()
    {
        left_sentinel = CreateLaser();
        evaluator = CreateLaser();
        right_sentinel = CreateLaser();
    }

    LaserMono CreateLaser() {
        GameObject go = Instantiate(LaserPrefab, gameObject.transform.position + new Vector3(0.0f, 0.0f, -5.0f), Quaternion.identity);
        
        return go.AddComponent<LaserMono>();
    }

    void SwitchTurnDirection() {
        if (this.turnDirection == TurnDirection.CCW) {
            this.turnDirection = TurnDirection.CW;
        } else {
            this.turnDirection = TurnDirection.CCW;
        }
    }

    void TurnEvaluator() {
        evaluator.Turn(TURN_SPEED * ((turnDirection == TurnDirection.CW)? -1.0f : 1.0f));
    }

    // Update is called once per frame
    void Update()
    {
        if (!areLasersConfigured) {
            left_sentinel.SetAngle(SENTINEL_ANGLE);
            right_sentinel.SetAngle(-SENTINEL_ANGLE);
            SetupDormantPhase();
            areLasersConfigured = true;
        } else {
            switch(systemState) {
                case SystemState.Dormant:
                    if (left_sentinel.isDetectingObstructor() || right_sentinel.isDetectingObstructor()) {
                        systemState = SystemState.Awake;
                        if (left_sentinel.isDetectingObstructor()) {
                            turnDirection = TurnDirection.CCW;
                        } else {
                            turnDirection = TurnDirection.CW;
                        }
                        left_sentinel.SetActiveState(false);
                        right_sentinel.SetActiveState(false);
                        evaluator.SetActiveState(true);
                    }
                    break;
                case SystemState.Awake:
                    if (isEvaluatorMoving) {
                        TurnEvaluator();
                    }
                    if ((evaluator.GetAngle() <= -SENTINEL_ANGLE) || (evaluator.GetAngle() >= SENTINEL_ANGLE)) {
                        oscillations += 1;
                        if (evaluator.GetAngle() <= -SENTINEL_ANGLE) {
                            turnDirection = TurnDirection.CCW;
                        } else {
                            turnDirection = TurnDirection.CW;
                        }
                    }
                    if ((evaluator.CheckReading() > previousReading)) {
                        if (isEvaluatorMoving) {
                            SwitchTurnDirection();
                            TurnEvaluator();
                            isEvaluatorMoving = false;
                        } else {
                            isEvaluatorMoving = true;
                        }
                    }
                    if (!isEvaluatorMoving && !evaluator.isDetectingObstructor()) {
                        isEvaluatorMoving = true;
                    }
                    if (evaluator.isDetectingObstructor()) {
                        oscillations = 0;
                    }
                    previousReading = evaluator.CheckReading();
                    if (oscillations == MAX_OSCILLATIONS) {
                        SetupDormantPhase();
                        systemState = SystemState.Dormant;
                    }
                    break;
                default:
                    // code block
                    break;
            }

        }
    }
}
