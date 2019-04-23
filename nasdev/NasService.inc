<?php
/*
	NasService.inc
	Copyright by xiemian
*/


/************************************
name:	NasSmbList
des:	service smb list process 
para:	
return:	
auth:	xiemian
date:	20111010
************************************/
function NasSmbList($m)
{
	global $config;
	
	$smb = array();

	if (!is_array($config['samba']))
	{
		$config['samba'] = array();
	}
		
	if(isset($config['samba']['enable']))
		$smb['status'] = "open";
	else
		$smb['status'] = "close";

	if($config['samba']['security'] === "share")
		$smb['authentication'] = "anonymous";
	else
		$smb['authentication'] = "local";

	$returnmsg = php_xmlrpc_encode($smb);
	return new xmlrpcresp($returnmsg);
}

/************************************
name:	NasSmbEdit
des:	service smb Edit process 
para:	
return:	
auth:	xiemian
date:	20111010
************************************/
function NasSmbEdit($m)
{
	global $g;
	
	$d_sysrebootreqd_path = $g['varrun_path'] . "/sysreboot.reqd";

	if (file_exists($d_sysrebootreqd_path))
	{
		$returnmsg = Nas2BMsg("fail","Please reboot NAS first!");		
		return new xmlrpcresp(php_xmlrpc_encode($returnmsg));		
	}

	global $config;

	$status = $m->getParam(0);
	$status = $status->scalarVal();
	$auth = $m->getParam(1);
	$auth = $auth->scalarVal();

	if($status === "open")
	{
		$config['samba']['enable'] = true;
	}
	else if($status === "close")
	{
		$config['samba']['enable'] = false;
	}
	else
	{
		$returnmsg = Nas2BMsg("fail","Invalid samba status!");
		return new xmlrpcresp(php_xmlrpc_encode($returnmsg));		
	}

	$config['samba']['netbiosname'] = "Nas";
	$config['samba']['workgroup'] = "WORKGROUP";
	$config['samba']['serverdesc'] = "Nas Server";

	if($auth === "anonymous")
	{
		$config['samba']['security'] = "share";
	}
	else if($auth === "local")
	{
		$config['samba']['security'] = "user";
	}
	else
	{
		$returnmsg = Nas2BMsg("fail","Invalid samba auth!");
		return new xmlrpcresp(php_xmlrpc_encode($returnmsg));		
	}
	
	$config['samba']['localmaster'] = "yes";
	$config['samba']['timesrv'] = "yes";
	$config['samba']['doscharset'] = "CP437";
	$config['samba']['unixcharset'] = "UTF-8";
	$config['samba']['loglevel'] = 1;
	$config['samba']['sndbuf'] = 64240;
	$config['samba']['rcvbuf'] = 64240;
	$config['samba']['largereadwrite'] = true;
	$config['samba']['usesendfile'] = true;
	$config['samba']['easupport'] = false;
	$config['samba']['storedosattributes'] = true;
	$config['samba']['guestaccount'] = "ftp";

	write_config();

	$retval = 0;
	
	config_lock();
	$retval |= rc_update_service("samba");
	$retval |= rc_update_service("mdnsresponder");
	config_unlock();

	if(!$retval)
	{
		$returnmsg = Nas2BMsg("success","Samba edit success!");
	}
	else
	{
		$returnmsg = Nas2BMsg("fail","Samba edit fail!");
	}

	return new xmlrpcresp(php_xmlrpc_encode($returnmsg));
	
}

/************************************
name:	NasNfsList
des:	service nfs list process 
para:	
return:	
auth:	xiemian
date:	20111010
************************************/
function NasNfsList($m)
{
	global $config;
	
	$nfsd = array();

	if (!is_array($config['nfsd']))
	{
		$config['nfsd'] = array();
	}
		
	if(isset($config['nfsd']['enable']))
		$nfsd['status'] = "open";
	else
		$nfsd['status'] = "close";

	$nfsd['number'] = $config['nfsd']['numproc'];

	$returnmsg = php_xmlrpc_encode($nfsd);
	return new xmlrpcresp($returnmsg);

}

/************************************
name:	NasNfsEdit
des:	service nfs Edit process 
para:	
return:	
auth:	xiemian
date:	20111010
************************************/
function NasNfsEdit($m)
{
	global $g;
	
	$d_sysrebootreqd_path = $g['varrun_path'] . "/sysreboot.reqd";

	if (file_exists($d_sysrebootreqd_path))
	{
		$returnmsg = Nas2BMsg("fail","Please reboot NAS first!");		
		return new xmlrpcresp(php_xmlrpc_encode($returnmsg));		
	}

	global $config;

	$status = $m->getParam(0);
	$status = $status->scalarVal();
	$number = $m->getParam(1);
	$number = $number->scalarVal();

	if($status === "open")
	{
		$config['nfsd']['enable'] = true;
	}
	else if($status === "close")
	{
		$config['nfsd']['enable'] = false;
	}
	else
	{
		$returnmsg = Nas2BMsg("fail","Invalid nfs status!");
		return new xmlrpcresp(php_xmlrpc_encode($returnmsg));		
	}

	if(($number !== 4) && ($number !== 5) && ($number !== 6))
	{
		$returnmsg = Nas2BMsg("fail","Invalid nfs server number!(4-6)");
		return new xmlrpcresp(php_xmlrpc_encode($returnmsg));
	}

	$config['nfsd']['numproc'] = $number;
	
	write_config();

	$retval = 0;
	config_lock();
	$retval |= rc_update_service("rpcbind"); // !!! Do
	$retval |= rc_update_service("mountd");  // !!! not
	$retval |= rc_update_service("nfsd");    // !!! change
	$retval |= rc_update_service("statd");   // !!! this
	$retval |= rc_update_service("lockd");   // !!! order
	$retval |= rc_update_service("mdnsresponder");
	config_unlock();	

	if(!$retval)
	{
		$returnmsg = Nas2BMsg("success","Nfs edit success!");
	}
	else
	{
		$returnmsg = Nas2BMsg("fail","Nfs edit fail!");
	}

	return new xmlrpcresp(php_xmlrpc_encode($returnmsg));
	
}

/************************************
name:	NasFtpList
des:	service ftp list process 
para:	
return:	
auth:	xiemian
date:	20111010
************************************/
function NasFtpList($m)
{
	global $config;
	
	$ftp = array();

	if (!is_array($config['ftpd']))
	{
		$config['ftpd'] = array();
	}
		
	if(isset($config['ftpd']['enable']))
		$ftp['status'] = "open";
	else
		$ftp['status'] = "close";

	$ftp['tcpport'] = $config['ftpd']['port'];
	$ftp['num'] = $config['ftpd']['numberclients'];

	if($config['ftpd']['localusersonly'] === true)
		$ftp['permission'] = "forbid";
	else
		$ftp['permission'] = "allow";
	
	$ftp['bandwidthmax'] = $config['ftpd']['userbandwidth']['up'];
	$ftp['bandwidthmin'] = $config['ftpd']['userbandwidth']['down'];

	$returnmsg = php_xmlrpc_encode($ftp);
	return new xmlrpcresp($returnmsg);
}

/************************************
name:	NasFtpEdit
des:	service ftp Edit process 
para:	
return:	
auth:	xiemian
date:	20111010
************************************/
function NasFtpEdit($m)
{
	global $g;
	
	$d_sysrebootreqd_path = $g['varrun_path'] . "/sysreboot.reqd";

	if (file_exists($d_sysrebootreqd_path))
	{
		$returnmsg = Nas2BMsg("fail","Please reboot NAS first!");		
		return new xmlrpcresp(php_xmlrpc_encode($returnmsg));		
	}

	global $config;

	$status = $m->getParam(0);
	$status = $status->scalarVal();
	$tcpport = $m->getParam(1);
	$tcpport = $tcpport->scalarVal();
	$number = $m->getParam(2);
	$number = $number->scalarVal();
	$permission = $m->getParam(3);
	$permission = $permission->scalarVal();
	$bandwidthmax = $m->getParam(4);
	$bandwidthmax = $bandwidthmax->scalarVal();
	$bandwidthmin = $m->getParam(5);
	$bandwidthmin = $bandwidthmin->scalarVal();

	if((!is_numericint($tcpport)) || (!is_numericint($number))
	|| (!is_numericint($bandwidthmax)) || (!is_numericint($bandwidthmin)))
	{
		$returnmsg = Nas2BMsg("fail","Invalid para type!");
		return new xmlrpcresp(php_xmlrpc_encode($returnmsg));
	}

	if ((1 > $number) || (50 < $number))
	{
		$returnmsg = Nas2BMsg("fail","Invalid client number!");
		return new xmlrpcresp(php_xmlrpc_encode($returnmsg));
	}

	if($status === "open")
	{
		$config['ftpd']['enable'] = true;
	}
	else if($status === "close")
	{
		$config['ftpd']['enable'] = false;
	}
	else
	{
		$returnmsg = Nas2BMsg("fail","Invalid ftp status!");
		return new xmlrpcresp(php_xmlrpc_encode($returnmsg));	
	}

	$config['ftpd']['port'] = $tcpport;
	$config['ftpd']['numberclients'] = $number;

	if($permission === "allow")
	{
		$config['ftpd']['localusersonly'] = false;
	}
	else if($permission === "forbid")
	{
		$config['ftpd']['localusersonly'] = true;
	}
	else
	{
		$returnmsg = Nas2BMsg("fail","Invalid ftp permission!");
		return new xmlrpcresp(php_xmlrpc_encode($returnmsg));		
	}	

	$config['ftpd']['userbandwidth']['up'] = $bandwidthmax;
	$config['ftpd']['userbandwidth']['down'] = $bandwidthmin;
	$config['ftpd']['anonymousbandwidth']['up'] = $bandwidthmax;
	$config['ftpd']['anonymousbandwidth']['down'] = $bandwidthmin;	

	write_config();

	$retval = 0;
	config_lock();
	$retval |= rc_update_service("proftpd");
	$retval |= rc_update_service("mdnsresponder");
	config_unlock();	

	if(!$retval)
	{
		$returnmsg = Nas2BMsg("success","Ftp edit success!");
	}
	else
	{
		$returnmsg = Nas2BMsg("fail","Ftp edit fail!");
	}

	return new xmlrpcresp(php_xmlrpc_encode($returnmsg));
	
}


/************************************
name:	NasIscsPgIgSet
des:	service iscsi pg ig set process 
para:	
return:	
auth:	xiemian
date:	20111010
************************************/
function NasIscsPgIgSet()
{
	global $config;

	if (!is_array($config['iscsitarget']['initiatorgroup']))
	{
		$config['iscsitarget']['initiatorgroup'] = array();

		array_sort_key($config['iscsitarget']['initiatorgroup'], "tag");

		$a_iscsitarget_ig = &$config['iscsitarget']['initiatorgroup'];

		$iscsitarget_ig = array();
		$iscsitarget_ig['uuid'] = uuid();
		$iscsitarget_ig['tag'] = 1;
		$iscsitarget_ig['comment'] = "";
		$iscsitarget_ig['iginitiatorname'] = "ALL";
		$iscsitarget_ig['ignetmask'] = "ALL";

		$a_iscsitarget_ig[] = $iscsitarget_ig;
	}

	if (!is_array($config['iscsitarget']['portalgroup']))
	{
		$config['iscsitarget']['portalgroup'] = array();

		array_sort_key($config['iscsitarget']['portalgroup'], "tag");
		
		$a_iscsitarget_pg = &$config['iscsitarget']['portalgroup'];

		$iscsitarget_pg = array();				
		$iscsitarget_pg['uuid'] = uuid();
		$iscsitarget_pg['tag'] = 1;
		$iscsitarget_pg['comment'] = "";
		$iscsitarget_pg['portal'] = "0.0.0.0:3260";

		$a_iscsitarget_pg[] = $iscsitarget_pg;
	}

}

/************************************
name:	NasIscsiList
des:	service iscsi list process 
para:	
return:	
auth:	xiemian
date:	20111010
************************************/
function NasIscsiList($m)
{
	global $config;

	$iscsi = array();
	$i = 0;

	if (!is_array($config['iscsitarget']))
	{
		$config['iscsitarget'] = array();
	}	

	if(isset($config['iscsitarget']['enable']))
		$iscsi['status'] = "open";
	else
		$iscsi['status'] = "close";
		
	if (!is_array($config['iscsitarget']['target']))
		$config['iscsitarget']['target'] = array();

	array_sort_key($config['iscsitarget']['target'], "name");

	if (!is_array($config['iscsitarget']['extent']))
		$config['iscsitarget']['extent'] = array();

	array_sort_key($config['iscsitarget']['extent'], "name");

	if (!is_array($config['iscsitarget']['authgroup']))
		$config['iscsitarget']['authgroup'] = array();

	array_sort_key($config['iscsitarget']['authgroup'], "tag");

	if (!is_array($config['volumes']['volume']))
		$config['volumes']['volume'] = array();

	array_sort_key($config['volumes']['volume'], "volumename");

	$a_volume = &$config['volumes']['volume'];
	$a_target = &$config['iscsitarget']['target'];
	$a_extent = &$config['iscsitarget']['extent'];
	$a_auth = &$config['iscsitarget']['authgroup'];
	
	foreach($a_target as $targetv)
	{
		$i++;
		$iscsi['iscsishare']['num'] = $i;
		$iscsi['iscsishare']['iscsi']['iscsi'.$i]['id'] = $targetv['uuid'];
		$iscsi['iscsishare']['iscsi']['iscsi'.$i]['iscsiname'] = $targetv['name'];

		$exentname = $targetv['lunmap'][0]['extentname'];
		$index = array_search_ex($exentname,$a_extent,"name");
		
		$iscsi['iscsishare']['iscsi']['iscsi'.$i]['type'] = $a_extent[$index]['type'];
		$iscsi['iscsishare']['iscsi']['iscsi'.$i]['member'] = $a_extent[$index]['volumename'];

		if($iscsi['iscsishare']['iscsi']['iscsi'.$i]['type'] === "file")
		{
			$iscsi['iscsishare']['iscsi']['iscsi'.$i]['size'] = $a_extent[$index]['size']."M";
		}
		else
		{
			if(false !== ($index = array_search_ex($a_extent[$index]['volumename'],$a_volume,"volumename")))
				$iscsi['iscsishare']['iscsi']['iscsi'.$i]['size'] = $a_volume[$index]['volumesize'];
			else
				$iscsi['iscsishare']['iscsi']['iscsi'.$i]['size'] = "auto";
		}

		$usertag = $targetv['agmap'][0]['agtag'];

		if(false !== ($index = array_search_ex($usertag,$a_auth,"tag")))
		{
			$iscsi['iscsishare']['iscsi']['iscsi'.$i]['user'] = $a_auth[$index]['agauth'][0]['authuser'];
		}
		else
		{
			$iscsi['iscsishare']['iscsi']['iscsi'.$i]['user'] = "ALL";
		}
	}
	
	$returnmsg = php_xmlrpc_encode($iscsi);
	return new xmlrpcresp($returnmsg);
}


/************************************
name:	NasIscsiSet
des:	service iscsi set process 
para:	
return:	
auth:	xiemian
date:	20111010
************************************/
function NasIscsiSet($m)
{
	global $g;
	
	$d_sysrebootreqd_path = $g['varrun_path'] . "/sysreboot.reqd";

	if (file_exists($d_sysrebootreqd_path))
	{
		$returnmsg = Nas2BMsg("fail","Please reboot NAS first!");		
		return new xmlrpcresp(php_xmlrpc_encode($returnmsg));		
	}

	global $config;

	$status = $m->getParam(0);
	$status = $status->scalarVal();

	if($status === "open")
	{
		$config['iscsitarget']['enable'] = true;
	}
	else if($status === "close")
	{
		$config['iscsitarget']['enable'] = false;
	}
	else
	{
		$returnmsg = Nas2BMsg("fail","Invalid iscsi status!");
		return new xmlrpcresp(php_xmlrpc_encode($returnmsg));	
	}

	write_config();

	$retval = 0;
	config_lock();
	$retval |= rc_update_service("iscsi_target");
	config_unlock();	

	if(!$retval)
	{
		$returnmsg = Nas2BMsg("success","Iscsi set success!");
	}
	else
	{
		$returnmsg = Nas2BMsg("fail","Iscsi set fail!");
	}

	return new xmlrpcresp(php_xmlrpc_encode($returnmsg));
}

/************************************
name:	NasIscsiDeviceMemberList
des:	service iscsi device member list process 
para:	
return:	
auth:	xiemian
date:	20111010
************************************/
function NasIscsiDeviceMemberList($m)
{
	global $config;

	$volume = array();
	$i = 0;

	if (!is_array($config['volumes']['volume']))
		$config['volumes']['volume'] = array();

	array_sort_key($config['volumes']['volume'], "volumename");

	if (!is_array($config['iscsitarget']['extent']))
		$config['iscsitarget']['extent'] = array();

	array_sort_key($config['iscsitarget']['extent'], "name");
	
	$a_volume = &$config['volumes']['volume'];
	$a_extent = &$config['iscsitarget']['extent'];

	foreach($a_volume as $volumev)
	{
		if($volumev['filesystem'] !== "ISCSI DEVICE")
			continue;

		if(false !== ($index = array_search_ex($volumev['volumename'],$a_extent,"volumename")))
			continue;
			
		$i++;
		$volume['iscsidevicemember'.$i] = $volumev['volumename']."(".$volumev['volumesize']."M)";
	}
	
	$returnmsg = php_xmlrpc_encode($volume);
	return new xmlrpcresp($returnmsg);	

}

/************************************
name:	NasIscsiFileMemberList
des:	service iscsi file member list process 
para:	
return:	
auth:	xiemian
date:	20111010
************************************/
function NasIscsiFileMemberList($m)
{
	global $config;

	$volume = array();
	$i = 0;
	$size = 0;

	if (!is_array($config['volumes']['volume']))
		$config['volumes']['volume'] = array();

	array_sort_key($config['volumes']['volume'], "volumename");

	if (!is_array($config['iscsitarget']['extent']))
		$config['iscsitarget']['extent'] = array();

	array_sort_key($config['iscsitarget']['extent'], "name");
	
	$a_volume = &$config['volumes']['volume'];
	$a_extent = &$config['iscsitarget']['extent'];

	foreach($a_volume as $volumev)
	{
		if($volumev['filesystem'] !== "ISCSI FILE")
			continue;
			
		$i++;
		$size = $volumev['volumesize'];

		foreach($a_extent as $extentv)
		{
			if($extentv['volumename'] === $volumev['volumename'])
			{
				$size -= $extentv['size'];
			}
		}

		$volume['iscsifilemember'.$i] = $volumev['volumename']."(".$size."M)";
	}
	
	$returnmsg = php_xmlrpc_encode($volume);
	return new xmlrpcresp($returnmsg);	
}

/************************************
name:	NasIscsiAdd
des:	service iscsi add process 
para:	
return:	
auth:	xiemian
date:	20111010
************************************/
function NasIscsiAdd($m)
{
	global $g;
	
	$d_sysrebootreqd_path = $g['varrun_path'] . "/sysreboot.reqd";

	if (file_exists($d_sysrebootreqd_path))
	{
		$returnmsg = Nas2BMsg("fail","Please reboot NAS first!");		
		return new xmlrpcresp(php_xmlrpc_encode($returnmsg));		
	}

	global $config;

	$name = $m->getParam(0);
	$name = $name->scalarVal();
	$member = $m->getParam(1);
	$member = $member->scalarVal();	
	$size = $m->getParam(2);
	$size = $size->scalarVal();	
	$user = $m->getParam(3);
	$user = $user->scalarVal();

	if (!is_array($config['iscsitarget']['target']))
		$config['iscsitarget']['target'] = array();

	array_sort_key($config['iscsitarget']['target'], "name");

	if (!is_array($config['iscsitarget']['extent']))
		$config['iscsitarget']['extent'] = array();

	array_sort_key($config['iscsitarget']['extent'], "name");

	if (!is_array($config['volumes']['volume']))
		$config['volumes']['volume'] = array();

	array_sort_key($config['volumes']['volume'], "volumename");

	if (!is_array($config['access']['user']))
		$config['access']['user'] = array();

	array_sort_key($config['access']['user'], "login");

	if (!is_array($config['mounts']['mount']))
		$config['mounts']['mount'] = array();

	array_sort_key($config['mounts']['mount'], "sharename");

	if (!is_array($config['disks']['disk']))
		$config['disks']['disk'] = array();

	array_sort_key($config['disks']['disk'], "name");
	
	$a_disk = &$config['disks']['disk'];	
	$a_mount = &$config['mounts']['mount'];
	$a_user = &$config['access']['user'];
	$a_volume = &$config['volumes']['volume'];
	$a_target = &$config['iscsitarget']['target'];
	$a_extent = &$config['iscsitarget']['extent'];

	if(false !== ($index = array_search_ex($name,$a_target,"name")))
	{
		$returnmsg = Nas2BMsg("fail","Repeat iscsi name!");
		return new xmlrpcresp(php_xmlrpc_encode($returnmsg));
	}

	if(false === ($cnid = array_search_ex($member,$a_volume,"volumename")))
	{
		$returnmsg = Nas2BMsg("fail","Invalid iscsi member!");
		return new xmlrpcresp(php_xmlrpc_encode($returnmsg));
	}

	$filesystem = $a_volume[$cnid]['filesystem'];

	if($filesystem === "ISCSI DEVICE")
	{
		if(false !== ($index = array_search_ex($member,$a_extent,"volumename")))
		{
			$returnmsg = Nas2BMsg("fail","Used iscsi member!");
			return new xmlrpcresp(php_xmlrpc_encode($returnmsg));
		}	
	}
	else if($filesystem === "ISCSI FILE")
	{
		$maxsize = $a_volume[$cnid]['volumesize'];

		foreach($a_extent as $extentv)
		{
			if($extentv['volumename'] === $a_volume[$cnid]['volumename'])
			{
				$maxsize -= $extentv['size'];
			}
		}

		if($size > $maxsize)
		{
			$returnmsg = Nas2BMsg("fail","No enough free space!");
			return new xmlrpcresp(php_xmlrpc_encode($returnmsg));
		}
	}
	else
	{
		$returnmsg = Nas2BMsg("fail","Invalid member type!");
		return new xmlrpcresp(php_xmlrpc_encode($returnmsg));
	}

	if(false === ($unid = array_search_ex($user,$a_user,"login")) && ($user !== "ALL"))
	{
		$returnmsg = Nas2BMsg("fail","Invalid user name!");
		return new xmlrpcresp(php_xmlrpc_encode($returnmsg));
	}

	NasIscsPgIgSet();
	
	if($user !== "ALL")
	{
		if (!is_array($config['iscsitarget']['authgroup']))
			$config['iscsitarget']['authgroup'] = array();

		array_sort_key($config['iscsitarget']['authgroup'], "tag");

		$a_auth = &$config['iscsitarget']['authgroup'];

		$tag = 1;
		$a_tags = array();
		foreach($a_auth as $ag)
			$a_tags[] = $ag['tag'];

		while(true === in_array($tag, $a_tags))
			$tag += 1;
			
		$iscsitarget_ag['uuid'] = uuid();
		$iscsitarget_ag['tag'] = $tag;
		$iscsitarget_ag['comment'] = "";
		$iscsitarget_ag['agauth'][0]['authuser'] = $user;
		$iscsitarget_ag['agauth'][0]['authsecret'] = $a_user[$unid]['password'];
		$iscsitarget_ag['agauth'][0]['authmuser'] = "";
		$iscsitarget_ag['agauth'][0]['authmsecret'] = "";

		$a_auth[] = $iscsitarget_ag;
	}

	$iscsitarget_extent['uuid'] = uuid();
	$iscsitarget_extent['name'] = $name;
	$iscsitarget_extent['volumename'] = $member;
	if($filesystem == "ISCSI DEVICE")
	{
		$iscsitarget_extent['type'] = "device";
		$iscsitarget_extent['size'] = "";
		$iscsitarget_extent['sizeunit'] = "auto";
		if($a_volume[$cnid]['volumetype'] == "SIMPLE")
		{
			$mnid = array_search_ex($a_volume[$cnid]['member'],$a_disk,"name");
			$iscsitarget_extent['path'] = $a_disk[$mnid]['devicespecialfile'];
		}
		else
		{
			$voltype = $a_volume[$cnid]['volumetype'];

			if($voltype == "JBOD")
				$voltag = "gconcat";
			if($voltype == "RAID 0")
				$voltag = "gstripe";
			if($voltype == "RAID 1")
				$voltag = "gmirror";
			if($voltype == "RAID 5")
				$voltag = "graid5";

			$a_vdisk = &$config[$voltag]['vdisk'];
			$mnid = array_search_ex($member,$a_vdisk,"name");	
			$iscsitarget_extent['path'] = $a_vdisk[$mnid]['devicespecialfile'];
		}
	}
	else
	{
		$iscsitarget_extent['type'] = "file";
		$iscsitarget_extent['size'] = $size;
		$iscsitarget_extent['sizeunit'] = "MB";
		$iscsitarget_extent['path'] = "/mnt/".$member."/".$name;
	}
	$iscsitarget_extent['comment'] = "";

	$a_extent[] = $iscsitarget_extent;

	$iscsitarget_target['uuid'] = uuid();
	$iscsitarget_target['enable'] = true;
	$iscsitarget_target['name'] = $name;
	$iscsitarget_target['alias'] = "";
	$iscsitarget_target['type'] = "disk";
	$iscsitarget_target['flags'] = "rw";
	$iscsitarget_target['comment'] = "";
	
	if($user !== "ALL")
	{
		$iscsitarget_target['authmethod'] = "CHAP";
		$iscsitarget_target['agmap'][0]['agtag'] = $tag;

	}
	else
	{
		$iscsitarget_target['authmethod'] = "Auto";
		$iscsitarget_target['agmap'][0]['agtag'] = 0;
	}

	$iscsitarget_target['digest'] = "Auto";
	$iscsitarget_target['queuedepth'] = 0;
	$iscsitarget_target['inqvendor'] = "";
	$iscsitarget_target['inqproduct'] = "";
	$iscsitarget_target['inqrevision'] = "";
	$iscsitarget_target['inqserial'] = "";
	$iscsitarget_target['blocklen'] = 512;
	$iscsitarget_target['pgigmap'][0]['pgtag'] = 1;
	$iscsitarget_target['pgigmap'][0]['igtag'] = 1;
	$iscsitarget_target['lunmap'][0]["lun"] = 0;
	$iscsitarget_target['lunmap'][0]["type"] = "Storage";
	$iscsitarget_target['lunmap'][0]["extentname"] = $name;

	$a_target[] = $iscsitarget_target;
	
	write_config();

	config_lock();
	$retval |= rc_update_service("iscsi_target");
	config_unlock();	

	if(!$retval)
	{
		$returnmsg = Nas2BMsg("success","Iscsi add success!");
	}
	else
	{
		$returnmsg = Nas2BMsg("fail","Iscsi add fail!");
	}

	return new xmlrpcresp(php_xmlrpc_encode($returnmsg));
}

/************************************
name:	NasIscsiEdit
des:	service iscsi edit process 
para:	
return:	
auth:	xiemian
date:	20111010
************************************/
function NasIscsiEdit($m)
{
	global $g;
	
	$d_sysrebootreqd_path = $g['varrun_path'] . "/sysreboot.reqd";

	if (file_exists($d_sysrebootreqd_path))
	{
		$returnmsg = Nas2BMsg("fail","Please reboot NAS first!");		
		return new xmlrpcresp(php_xmlrpc_encode($returnmsg));		
	}

	global $config;

	$uuid = $m->getParam(0);
	$uuid = $uuid->scalarVal();
	$member = $m->getParam(1);
	$member = $member->scalarVal();	
	$size = $m->getParam(2);
	$size = $size->scalarVal();	
	$user = $m->getParam(3);
	$user = $user->scalarVal();
	
	if (!is_array($config['iscsitarget']['target']))
		$config['iscsitarget']['target'] = array();

	array_sort_key($config['iscsitarget']['target'], "name");

	if (!is_array($config['iscsitarget']['extent']))
		$config['iscsitarget']['extent'] = array();

	array_sort_key($config['iscsitarget']['extent'], "name");

	if (!is_array($config['iscsitarget']['authgroup']))
		$config['iscsitarget']['authgroup'] = array();

	array_sort_key($config['iscsitarget']['authgroup'], "tag");
	
	if (!is_array($config['volumes']['volume']))
		$config['volumes']['volume'] = array();

	array_sort_key($config['volumes']['volume'], "volumename");

	if (!is_array($config['access']['user']))
		$config['access']['user'] = array();

	array_sort_key($config['access']['user'], "login");

	if (!is_array($config['mounts']['mount']))
		$config['mounts']['mount'] = array();

	array_sort_key($config['mounts']['mount'], "sharename");

	if (!is_array($config['disks']['disk']))
		$config['disks']['disk'] = array();

	array_sort_key($config['disks']['disk'], "name");
	
	$a_disk = &$config['disks']['disk'];	
	$a_mount = &$config['mounts']['mount'];
	$a_user = &$config['access']['user'];
	$a_volume = &$config['volumes']['volume'];
	$a_target = &$config['iscsitarget']['target'];
	$a_extent = &$config['iscsitarget']['extent'];
	$a_auth = &$config['iscsitarget']['authgroup'];

	if(false === ($tnid = array_search_ex($uuid,$a_target,"uuid")))
	{
		$returnmsg = Nas2BMsg("fail","Error scsi id!");
		return new xmlrpcresp(php_xmlrpc_encode($returnmsg));
	}

	$name = $a_target[$tnid]['name'];
	$tag = $a_target[$tnid]['agmap'][0]['agtag'];
	$enid = array_search_ex($name,$a_extent,"name");

	if(false === ($cnid = array_search_ex($member,$a_volume,"volumename")))
	{
		$returnmsg = Nas2BMsg("fail","Invalid iscsi member!");
		return new xmlrpcresp(php_xmlrpc_encode($returnmsg));
	}

	$filesystem = $a_volume[$cnid]['filesystem'];

	if($filesystem === "ISCSI DEVICE")
	{
		if((false !== ($index = array_search_ex($member,$a_extent,"volumename")))
		 && ($a_extent[$index]['name'] !== $name))
		{
			$returnmsg = Nas2BMsg("fail","Used iscsi member!");
			return new xmlrpcresp(php_xmlrpc_encode($returnmsg));
		}	
	}
	else if($filesystem === "ISCSI FILE")
	{
		$maxsize = $a_volume[$cnid]['volumesize'];

		if($a_extent[$enid]['type'] === "device")
			$selfsize = 0;
		else
			$selfsize = $a_extent[$enid]['size'];

		foreach($a_extent as $extentv)
		{
			if($extentv['volumename'] === $a_volume[$cnid]['volumename'])
			{
				$maxsize -= $extentv['size'];
			}
		}

		$maxsize += $selfsize;

		if($size > $maxsize)
		{
			$returnmsg = Nas2BMsg("fail","No enough free space!");
			return new xmlrpcresp(php_xmlrpc_encode($returnmsg));
		}
	}
	else
	{
		$returnmsg = Nas2BMsg("fail","Invalid member type!");
		return new xmlrpcresp(php_xmlrpc_encode($returnmsg));
	}	
	
	if(false === ($unid = array_search_ex($user,$a_user,"login")) && ($user !== "ALL"))
	{
		$returnmsg = Nas2BMsg("fail","Invalid user name!");
		return new xmlrpcresp(php_xmlrpc_encode($returnmsg));
	}

	$a_extent[$enid]['volumename'] = $member;
	
	if($filesystem == "ISCSI DEVICE")
	{
		$a_extent[$enid]['type'] = "device";
		$a_extent[$enid]['size'] = "";
		$a_extent[$enid]['sizeunit'] = "auto";
		if($a_volume[$cnid]['volumetype'] == "SIMPLE")
		{
			$mnid = array_search_ex($a_volume[$cnid]['member'],$a_disk,"name");
			$a_extent[$enid]['path'] = $a_disk[$mnid]['devicespecialfile'];
		}
		else
		{
			$mnid = array_search_ex($member,$a_mount,"sharename");	
			$a_extent[$enid]['path'] = $a_mount[$mnid]['mdisk'];
		}
	}
	else
	{
		$a_extent[$enid]['type'] = "file";
		$a_extent[$enid]['size'] = $size;
		$a_extent[$enid]['sizeunit'] = "MB";
		$a_extent[$enid]['path'] = "/mnt/".$member."/".$name;
	}

	if($user !== "ALL")
	{
		if($tag != 0)
		{
			$anid = array_search_ex($tag,$a_auth,"tag");

			$a_auth[$anid]['agauth'][0]['authuser'] = $user;
			$a_auth[$anid]['agauth'][0]['authsecret'] = $a_user[$unid]['password'];
		}
		else
		{
			$tag = 1;
			$a_tags = array();
			foreach($a_auth as $ag)
				$a_tags[] = $ag['tag'];

			while(true === in_array($tag, $a_tags))
				$tag += 1;
				
			$iscsitarget_ag['uuid'] = uuid();
			$iscsitarget_ag['tag'] = $tag;
			$iscsitarget_ag['comment'] = "";
			$iscsitarget_ag['agauth'][0]['authuser'] = $user;
			$iscsitarget_ag['agauth'][0]['authsecret'] = $a_user[$unid]['password'];
			$iscsitarget_ag['agauth'][0]['authmuser'] = "";
			$iscsitarget_ag['agauth'][0]['authmsecret'] = "";

			$a_auth[] = $iscsitarget_ag;
			$a_target[$tnid]['authmethod'] = "CHAP";
			$a_target[$tnid]['agmap'][0]['agtag'] = $tag;
		}
	}
	else
	{
		if($tag != 0)
		{
			$anid = array_search_ex($tag,$a_auth,"tag");

			unset($a_auth[$anid]);
			$a_target[$tnid]['authmethod'] = "Auto";
			$a_target[$tnid]['agmap'][0]['agtag'] = 0;
		}
		else
		{
		}
	}

	write_config();

	config_lock();
	$retval |= rc_update_service("iscsi_target");
	config_unlock();	

	if(!$retval)
	{
		$returnmsg = Nas2BMsg("success","Iscsi edit success!");
	}
	else
	{
		$returnmsg = Nas2BMsg("fail","Iscsi edit fail!");
	}

	return new xmlrpcresp(php_xmlrpc_encode($returnmsg));
}


/************************************
name:	NasIscsiDel
des:	service iscsi del process 
para:	
return:	
auth:	xiemian
date:	20111010
************************************/
function NasIscsiDel($m)
{
	global $g;
	
	$d_sysrebootreqd_path = $g['varrun_path'] . "/sysreboot.reqd";

	if (file_exists($d_sysrebootreqd_path))
	{
		$returnmsg = Nas2BMsg("fail","Please reboot NAS first!");		
		return new xmlrpcresp(php_xmlrpc_encode($returnmsg));		
	}

	global $config;

	$uuid = $m->getParam(0);
	$uuid = $uuid->scalarVal();

	if (!is_array($config['iscsitarget']['target']))
		$config['iscsitarget']['target'] = array();

	array_sort_key($config['iscsitarget']['target'], "name");

	if (!is_array($config['iscsitarget']['extent']))
		$config['iscsitarget']['extent'] = array();

	array_sort_key($config['iscsitarget']['extent'], "name");

	if (!is_array($config['iscsitarget']['authgroup']))
		$config['iscsitarget']['authgroup'] = array();

	array_sort_key($config['iscsitarget']['authgroup'], "tag");
	
	$a_target = &$config['iscsitarget']['target'];
	$a_extent = &$config['iscsitarget']['extent'];
	$a_auth = &$config['iscsitarget']['authgroup'];

	if(false === ($tnid = array_search_ex($uuid,$a_target,"uuid")))
	{
		$returnmsg = Nas2BMsg("fail","Error scsi id!");
		return new xmlrpcresp(php_xmlrpc_encode($returnmsg));
	}

	$name = $a_target[$tnid]['name'];
	$tag = $a_target[$tnid]['agmap'][0]['agtag'];
	
	$enid = array_search_ex($name,$a_extent,"name");

	unset($a_target[$tnid]);
	unset($a_extent[$enid]);
	
	if($tag != 0)
	{
		$anid = array_search_ex($tag,$a_auth,"tag");
		unset($a_auth[$anid]);
	}

	write_config();

	config_lock();
	$retval |= rc_update_service("iscsi_target");
	config_unlock();	

	if(!$retval)
	{
		$returnmsg = Nas2BMsg("success","Iscsi del success!");
	}
	else
	{
		$returnmsg = Nas2BMsg("fail","Iscsi del fail!");
	}

	return new xmlrpcresp(php_xmlrpc_encode($returnmsg));
}



?>
