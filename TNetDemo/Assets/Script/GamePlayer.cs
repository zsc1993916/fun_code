using UnityEngine;
using System.Collections;
using TNet;

public class GamePlayer : TNBehaviour {

	public static GamePlayer _instance;
	
	public float Speed = 5f;

	private Vector3 _target;
	public Vector3 target
	{
		set
		{
			tno.Send("OnSetTarget", TNet.Target.AllSaved, value);
		}
	}
	[RFC]
	void OnSetTarget(Vector3 v)
	{
		this._target = v;
	}
	void OnNetworkPlayerJoin(Player p)
	{
		tno.Send("OnSetTargetImmediate", TNet.Target.AllSaved, transform.position);
	}
	[RFC]
	void OnSetTargetImmediate(Vector3 v)
	{
		this.transform.position = v;
	}


	void Awake()
	{
		//只能操作自己实例化的Player
		if (TNManager.isThisMyObject)
		{
			GamePlayer._instance = this;
		}
	}
	void Update()
	{
		this.transform.position = Vector3.Lerp(this.transform.position, this._target, Time.deltaTime * Speed);
	}
}
