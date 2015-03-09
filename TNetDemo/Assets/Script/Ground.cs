using UnityEngine;
using System.Collections;

public class Ground : MonoBehaviour {


	void OnClick() {
		GamePlayer._instance.target = TouchHandler.worldPos;
	}
}
