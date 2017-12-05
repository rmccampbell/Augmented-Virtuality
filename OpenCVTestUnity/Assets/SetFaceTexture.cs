using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class SetFaceTexture : MonoBehaviour {

	// Use this for initialization
	void Start () {
        GetComponent<Renderer>().material.mainTexture = OpenCVFaceDetection.FaceTexture;
    }

    // Update is called once per frame
    void Update () {
		
	}
}
