'use strict';

var localVideo = document.getElementById("localVideo");
var pushBtn = document.getElementById("pushBtn");
var stopPushBtn = document.getElementById("stopPushBtn");

pushBtn.addEventListener("click", startPush);
stopPushBtn.addEventListener("click", stopPush);

var uid = $("#uid").val();
var streamName = $("#streamName").val();
var audio = $("#audio").val();
var video = $("#video").val();
var offer = "";
var pc;
const config = {};
var localStream;
var lastConnectionState = "";

function startPush() {
    console.log("发送推流请求: /signaling/push");

    $.post("/signaling/push",
        {"uid": uid, "streamName": streamName, "audio": audio, "video": video},
        function(data, textStatus) {
            console.log("推流响应: " + JSON.stringify(data));
            if ("success" == textStatus && 0 == data.errNo) {
                $("#tips1").html("<font color='blue'>推流请求成功!</font>");
                console.log("offer sdp: \n" + data.data.sdp);
                offer = data.data;
                pushStream();
            } else {
                $("#tips1").html("<font color='red'>推流请求失败!</font>");
            }
        },
        "json"
    );
}


function stopPush() {
    console.log("send stop push: /signaling/stoppush");

    localVideo.srcObject = null;
    if (localStream && localStream.getAudioTracks()) {
        localStream.getAudioTracks()[0].stop();
    }

    if (localStream && localStream.getVideoTracks()) {
        localStream.getVideoTracks()[0].stop();
    }

    if (pc) {
        pc.close();
        pc = null;
    }

    $("#tips1").html("");
    $("#tips2").html("");
    $("#tips3").html("");

    $.post("/signaling/stoppush",
        {"uid": uid, "streamName": streamName},
        function(data, textStatus) {
            console.log("stop push response: " + JSON.stringify(data));
            if ("success" == textStatus && 0 == data.errNo) {
                $("#tips1").html("<font color='blue'>停止推流请求成功!</font>");
            } else {
                $("#tips1").html("<font color='red'>停止推流请求失败!</font>");
            }
        },
        "json"
    );

}

function sendAnswer(answerSdp) {
	
    console.log("send answer: /signaling/sendanswer");

    $.post("/signaling/sendanswer",
        {"uid": uid, "streamName": streamName, "answer": answerSdp, "type": "push"},
        function(data, textStatus) {
            console.log("send answer sdp: " + JSON.stringify(data));
            if ("success" == textStatus && 0 == data.errNo) {
                $("#tips3").html("<font color='blue'>answer 发送成功!</font>");
            } else {
                $("#tips3").html("<font color='red'>answer 发送失败!</font>");
            }
        },
        "json"
	);
	
}

function pushStream() {
    pc = new RTCPeerConnection(config);

	pc.oniceconnectionstatechange = function(e) {
		var state = "";
		if(lastConnectionState != "") {
			state =lastConnectionState +"->" + pc.iceConnectionState;
		} else {
			state =pc.iceConnectionState;
		}

		$("#tips2").html("连接状态"+state);
		lastConnectionState = pc.iceConnectionState;
		
	}

    pc.setRemoteDescription(offer).then(
        setRemoteDescriptionSuccess,
        setRemoteDescriptionError
    );
}

function startCameraStream() {
    var constraints = {
        audio: true,
        video: true
    };

    navigator.mediaDevices.getUserMedia(constraints).then(
        handleSuccess).catch(handleError);
}

function handleSuccess(stream) {
    localVideo.srcObject = stream;
    localStream = stream;
    pc.addStream(stream);
    pc.createAnswer().then(
        createSessionDescriptionSuccess,
        createSessionDescriptionError               
    );
}

function handleError(error) {
    console.log("获取媒体设备错误: " + error);
}

function setRemoteDescriptionSuccess() {
    console.log("set remote sdp success");
    startCameraStream();
}

function createSessionDescriptionSuccess(answer) {
    console.log("answer sdp: \n" + answer.sdp);
	console.log("pc set local sdp");
	pc.setLocalDescription(answer).then(
		setLocalDescriptionSuccess,
		setLocalDescriptionError
	);

	sendAnswer(answer.sdp);

	
}

function setLocalDescriptionSuccess() {
	console.log("set local description success");
}

function setLocalDescriptionError() {
	console.log("set local description error:" + error);
}

function setRemoteDescriptionError(error) {
    console.log("设置远程描述错误: " + error);
}

function createSessionDescriptionError(error) {
    console.log("创建应答错误: " + error);
}
