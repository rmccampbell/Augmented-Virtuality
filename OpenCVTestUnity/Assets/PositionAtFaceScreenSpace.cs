using UnityEngine;

public class PositionAtFaceScreenSpace : MonoBehaviour
{
    private float _camDistance;

    void Start()
    {
        _camDistance = Vector3.Distance(Camera.main.transform.position, transform.position);
    }

    void Update()
    {
        if (OpenCVFaceDetection.FacePosition == Vector2.zero)
            return;

        transform.position = Camera.main.ViewportToWorldPoint(new Vector3(OpenCVFaceDetection.FacePosition.x, OpenCVFaceDetection.FacePosition.y, _camDistance));
    }
}
