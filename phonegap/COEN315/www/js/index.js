/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */
var app = {
    // Application Constructor
    initialize: function() {
        this.bindEvents();
    },
    // Bind Event Listeners
    //
    // Bind any events that are required on startup. Common events are:
    // 'load', 'deviceready', 'offline', and 'online'.
    bindEvents: function() {
        document.addEventListener('deviceready', this.onDeviceReady, false);
    },
    // deviceready Event Handler
    //
    // The scope of 'this' is the event. In order to call the 'receivedEvent'
    // function, we must explicitly call 'app.receivedEvent(...);'
    onDeviceReady: function() {
        app.receivedEvent('deviceready');
        $("#loginForm").on("submit",handleLogin);
    },
    // Update DOM on a Received Event
    receivedEvent: function(id) {
        var parentElement = document.getElementById(id);
        var listeningElement = parentElement.querySelector('.listening');
        var receivedElement = parentElement.querySelector('.received');

        listeningElement.setAttribute('style', 'display:none;');
        receivedElement.setAttribute('style', 'display:block;');

        console.log('Received Event: ' + id);
        get_pressed();
    }
};
//code from https://www.raymondcamden.com/2011/11/10/Example-of-serverbased-login-with-PhoneGap
function handleLogin() {
	var form = $("#loginForm");	
	//disable the button so we can't resubmit while we wait
	$("#submitButton",form).attr("disabled","disabled");
	var u = $("#username", form).val();
	var p = $("#password", form).val();
	console.log("click");
	if(u != '' && p!= '') {
		$.get("http://bart.student-t-test.com/stations.php", {username:u,password:p}, function(res) {
			if(res) {
        		//just go
				window.location = "actual.html";
			} else {
				navigator.notification.alert("Your login failed", function() {});
			}
	    	$("#submitButton").removeAttr("disabled");
		},"json");
	} else {
		//Thanks Igor!
		navigator.notification.alert("You must enter a username and password", function() {});
		$("#submitButton").removeAttr("disabled");
	}
	return false;
}

function get_pressed() {
	$.get("http://bart.student-t-test.com/stations.php", function(res) {
        $("#buttonpressed").html(res);
	},"json");
	return false;
}
