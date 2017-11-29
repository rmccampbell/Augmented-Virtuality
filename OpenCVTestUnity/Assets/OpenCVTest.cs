using System.Collections;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using UnityEngine;

public class OpenCVTest : MonoBehaviour {

    [DllImport("OpenCVTest", CallingConvention = CallingConvention.Cdecl)]
    public static extern void Initialize();

    [DllImport("OpenCVTest", CallingConvention = CallingConvention.Cdecl)]
    public static extern byte[] ProcessFrame();

    Texture2D tex;

    private void Start()
    {
        Initialize();
        tex = new Texture2D(640, 480, TextureFormat.BGRA32, false);
        GetComponent<Renderer>().material.mainTexture = tex;
    }

    private void Update()
    {
        byte[] imgData = ProcessFrame();
        tex.LoadRawTextureData(imgData);
        tex.Apply();
    }
}
