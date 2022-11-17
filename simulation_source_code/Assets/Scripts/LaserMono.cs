using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class LaserMono : MonoBehaviour
{
    private const float LASER_WIDTH = 0.3f;
    private const float MAX_LASER_RANGE = 18.0f;
    private readonly Color ACTIVATED_COLOR = new Color(1.0f, 0.0f, 0.0f, 1.0f);
    private readonly Color DEACTIVATED_COLOR = new Color(0.0f, 0.0f, 0.0f, 0.25f);
    private float currentReading;
    private float currentAngle;
    private bool isActive;
    private SpriteRenderer spriteRenderer;
    private Transform laserSpriteObject;

    public void SetAngle(float angle) {
        currentAngle = angle;
        gameObject.transform.rotation = Quaternion.Euler(0.0f, 0.0f, currentAngle);
    }

    public float GetAngle() { return currentAngle; }

    public void Turn(float angle) {
        currentAngle += angle;
        gameObject.transform.rotation = Quaternion.Euler(0.0f, 0.0f, currentAngle);
    }

    public void SetDisplayRange(float range) {
        laserSpriteObject.localScale = new Vector3(1.0f, range/MAX_LASER_RANGE, 1.0f);
    }

    public void SetActiveState(bool activeState) {
        isActive = activeState;
    }

    public float CheckReading() {
        if (isActive) {
            return currentReading;
        } else {
            return 0.0f;
        }
    }

    public bool isDetectingObstructor() {
        return currentReading != MAX_LASER_RANGE;
    }

    public void SetCurrentReading(float val) {
        currentReading = val;
        SetDisplayRange(currentReading);
    }

    void Start()
    {
        laserSpriteObject = gameObject.transform.GetChild(0);
        this.spriteRenderer = laserSpriteObject.GetComponent<SpriteRenderer>();
        gameObject.transform.localScale = new Vector3(LASER_WIDTH, MAX_LASER_RANGE, 1.0f);
        
        SetActiveState(true);
        SetDisplayRange(MAX_LASER_RANGE);
        Debug.Log("good morning sunshine!");
    }

    // Update is called once per frame
    void Update()
    {
        if (isActive) {
            this.spriteRenderer.color = ACTIVATED_COLOR;

            Vector3 currentAngleVector = new Vector3(Mathf.Cos((currentAngle + 90.0f)*Mathf.Deg2Rad), Mathf.Sin((currentAngle + 90.0f)*Mathf.Deg2Rad),0.0f);
            RaycastHit2D hit = Physics2D.Raycast(transform.position, currentAngleVector);
            if (hit.collider != null)
            {
                float distance = Vector2.Distance(hit.point, transform.position);
                if (distance < MAX_LASER_RANGE) {
                    SetCurrentReading(distance);
                } else {
                    SetCurrentReading(MAX_LASER_RANGE);
                }
            } else {
                SetCurrentReading(MAX_LASER_RANGE);
            }
        } else {
            currentReading = MAX_LASER_RANGE;
            this.spriteRenderer.color = DEACTIVATED_COLOR;
        }
    }
}
