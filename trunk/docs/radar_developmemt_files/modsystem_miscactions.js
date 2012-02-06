var xmlhttp;
var nombottomDiv;

function XHCon(sURL, sMethod, sVars)
{
	xmlhttp = false;
	if(window.XMLHttpRequest)
	{
		try {
			xmlhttp = new XMLHttpRequest();
		} catch(e) {
			xmlhttp = false;
		}
	}
	else if(window.ActiveXObject)
	{
		try {
			xmlhttp = new ActiveXObject("Msxml2.XMLHTTP");
		} catch(e) {
			try {
				xmlhttp = new ActiveXObject("Microsoft.XMLHTTP");
			} catch(e) {
				xmlhttp = false;
			}
		}
	}

	if (xmlhttp)
	{
		sMethod = sMethod.toUpperCase();
		xmlhttp.onreadystatechange = processReqChange;
		if (sMethod == "GET")
		{
			xmlhttp.open(sMethod, sURL + "?" + sVars, true);
			sVars = "";
		}
		else
		{
			xmlhttp.open(sMethod, sURL, true);
			xmlhttp.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");
		}
		xmlhttp.send(sVars);
		return true;
	}
	return false;
}

function processReqChange()
{
	if (xmlhttp.readyState == 4)
	{
		if (xmlhttp.status == 200)
		{
			if (xmlhttp.responseText)
			{
				switch(xmlhttp.responseText)
				{
					case 'installed':
						if (script == 'showthread')
						{
							bottomDiv.innerHTML = nombottomDiv_nominate_a + ' <a href="javascript: mods_install(' + threadid + ',false)">' + bottomdiv_uninstalled + '</a>.';
							sideDiv.innerHTML = '<a href="javascript: mods_install(' + threadid + ',false)"> ' + bottomdiv_uninstalled + ' </a>';
							installDiv.innerHTML = parseFloat(installDiv.innerHTML) + 1;
						}
						else
						{
							threadInstalled.innerHTML = '<a style="color:#008800;" href="javascript: mods_install(' + threadSave + ', false)">' + threadInstalled_installed + ':</a>';
							threadInstallCount.className = 'highlight';
							threadInstallCount.innerHTML = parseFloat(threadInstallCount.innerHTML) + 1;
						}
						break;
					case 'uninstalled':
						if (script == 'showthread')
						{
							bottomDiv.innerHTML = bottomdiv_to_receive + ' <a href="javascript: mods_install(' + threadid + ',true)">' + bottomdiv_installed + '</a>.';
							sideDiv.innerHTML = '<a href="javascript: mods_install(' + threadid + ',true)"> ' + bottomdiv_installed + ' </a>' ;
							installDiv.innerHTML = parseFloat(installDiv.innerHTML) - 1;
						}
						else
						{
							threadInstalled.innerHTML = '<a style="color:#999999;" href="javascript: mods_install(' + threadSave + ', true)">' + threadInstalled_uninstalled + ':</a>';
							threadInstallCount.className = 'smallfont';
							threadInstallCount.innerHTML = parseFloat(threadInstallCount.innerHTML) - 1;
						}
						break;
			  	case 'nominated':
						nombottomDiv.innerHTML = nombottomDiv_nominate_a + ' <a href="javascript: mods_nominate(' + threadid + ',1,false)">' + nombottomDiv_nominate_b;
						nomsideDiv.innerHTML = '<a href="javascript: mods_nominate(' + threadid + ',1,false)">' + nomsideDiv_nominate_clear + '</a>' ;
						break;
					case 'unnominated':
						nombottomDiv.innerHTML = nombottomDiv_unnominate_a + ' <a href="javascript: mods_nominate(' + threadid + ',1,true)">' + nombottomDiv_unnominate_b;
						nomsideDiv.innerHTML = '<a href="javascript: mods_nominate(' + threadid + ',1,true)">' + nomsideDiv_nominate + '</a>' ;
						break;
					default:
						break;
				}
			}
		}
		else
		{
		}
	}
}

// Sends request to install a modification
function mods_install(threadid, install)
{
	threadid = PHP.urlencode(threadid);	
	threadSave = threadid;
	
	if (script == 'showthread')
	{
		bottomDiv = fetch_object('mods_install_div');
	    sideDiv = fetch_object('mods_install_sidebar');
	 installDiv = fetch_object('mods_install_count');
	}
	else
	{
		threadInstalled = fetch_object('thread_installed_' + threadid);
		threadInstallCount = fetch_object('thread_installcount_' + threadid);
	}

	//send the request to php script...
	dostring = (install) ? 'installmod' : 'uninstallmod';
	XHCon("modsystem_miscactions.php", "POST", SESSIONURL + '&securitytoken=' + SECURITYTOKEN + '&do=' + dostring + '&ajax=1&threadid=' + threadid);
}

// Sends request to nominate a modification
function mods_nominate(threadid, type, nominate)
{
	type = PHP.urlencode(type);
	threadid = PHP.urlencode(threadid);
	
	nombottomDiv = fetch_object('mods_nominate_div');
    nomsideDiv = fetch_object('mods_nominate_sidebar');

	//send the request to php script...
	dostring = (nominate) ? 'nominate' : 'unnominate';
	XHCon("modsystem_miscactions.php", "POST", SESSIONURL + '&securitytoken=' + SECURITYTOKEN + '&do=' + dostring + '&ajax=1&type=' + type + '&threadid=' + threadid);
}	
	
