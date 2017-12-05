using System.Collections;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using UnityEngine;

public class OpenCVFaceDetection : MonoBehaviour
{
    public static Vector2Int CameraResolution;
    public static Vector2 FacePosition;
    public static Texture2D FaceTexture;

    /// <summary>
    /// Downscale factor to speed up detection.
    /// </summary>
    private const int DetectionDownScale = 1;

    private bool _ready;
    byte[] _pixels;

    void Awake()
    {
        int camWidth = 0, camHeight = 0;
        int result = OpenCVInterop.Init(out camWidth, out camHeight);
        if (result < 0)
        {
            if (result == -1)
            {
                Debug.LogWarningFormat("[{0}] Failed to find cascades definition.", GetType());
            }
            else if (result == -2)
            {
                Debug.LogWarningFormat("[{0}] Failed to open camera stream.", GetType());
            }

            return;
        }

        CameraResolution = new Vector2Int(camWidth, camHeight);
        _pixels = new byte[camWidth * camHeight * 4];
        FaceTexture = new Texture2D(camWidth, camHeight, TextureFormat.BGRA32, false);

        OpenCVInterop.SetScale(DetectionDownScale);
        _ready = true;
    }

    void OnApplicationQuit()
    {
        if (_ready)
        {
            OpenCVInterop.Close();
        }
    }

    void Update()
    {
        if (!_ready)
            return;

        CvCircle face;
        int ret;
        unsafe
        {
            fixed (byte* outPixels = _pixels)
            {
                ret = OpenCVInterop.Detect(out face, outPixels, _pixels.Length);
            }
        }

        if (ret > 0)
        {
            FacePosition = new Vector2((float)face.X / CameraResolution.x, 1f - ((float)face.Y / CameraResolution.y));
            FaceTexture.LoadRawTextureData(_pixels);
            FaceTexture.Apply();
        }
    }
}

// Define the functions which can be called from the .dll.
internal static class OpenCVInterop
{
    [DllImport("OpenCVTest2")]
    internal static extern int Init(out int outCameraWidth, out int outCameraHeight);

    [DllImport("OpenCVTest2")]
    internal static extern int Close();

    [DllImport("OpenCVTest2")]
    internal static extern int SetScale(int downscale);

    [DllImport("OpenCVTest2")]
    internal unsafe static extern int Detect(out CvCircle outFace, byte *outPixels, int outPixelsSize);
}

// Define the structure to be sequential and with the correct byte size (3 ints = 4 bytes * 3 = 12 bytes)
[StructLayout(LayoutKind.Sequential, Size = 12)]
public struct CvCircle
{
    public int X, Y, Radius;
}
