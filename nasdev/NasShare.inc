<?php
/*
	NasShare.inc
	Copyright by xiemian
*/

/************************************
name:	NasWinShareList
des:	win share list process 
para:	
return:	
auth:	xiemian
date:	20111010
************************************/
function NasWinShareList($m)
{
	global $config;
	
	$share = array();
	$i = 0;

	if (!is_array($config['samba']['share']))
		$config['samba']['share'] = array();

	array_sort_key($config['samba']['share'], "name");
	array_sort_key($config['mounts']['mount'], "devicespecialfile");
	
	$a_share = &$config['samba']['share'];
		
	foreach($a_share as $sharev)
	{
		$i++;
		$share['num'] = $i;
		$share['winshare']['winshare'.$i]['id'] = $sharev['uuid'];
		$share['winshare']['winshare'.$i]['sharename'] = $sharev['name'];
		$share['winshare']['winshare'.$i]['volume'] = $sharev['volumemember'];
		$share['winshare']['winshare'.$i]['desc'] = $sharev['comment'];
	}
	
	$returnmsg = php_xmlrpc_encode($share);
	return new xmlrpcresp($returnmsg);
}

/************************************
name:	NasShareMemberList
des:	share member list 
para:	
return:	
auth:	xiemian
date:	20111010
************************************/
function NasShareMemberList($m)
{
	global $config;
	
	$share = array();
	$i = 0;
	
	if (!is_array($config['volumes']['volume']))
		$config['volumes']['volume'] = array();
		
	array_sort_key($config['volumes']['volume'], "volumename");

	$a_volume = $config['volumes']['volume'];

	foreach($a_volume as $volumev)
	{
		if($volumev['filesystem'] !== "SHARE")
			continue;

		$i++;
		$share['sharemember'.$i] = $volumev['volumename'];
	}
	
	$returnmsg = php_xmlrpc_encode($share);
	return new xmlrpcresp($returnmsg);
}

/************************************
name:	NasWinShareAdd
des:	win share add process 
para:	
return:	
auth:	xiemian
date:	20111010
************************************/
function NasWinShareAdd($m)
{
	global $g;
	
	$d_sysrebootreqd_path = $g['varrun_path'] . "/sysreboot.reqd";

	if (file_exists($d_sysrebootreqd_path))
	{
		$returnmsg = Nas2BMsg("fail","Please reboot NAS first!");		
		return new xmlrpcresp(php_xmlrpc_encode($returnmsg));		
	}

	global $config;

	$sharename = $m->getParam(0);
	$sharename = $sharename->scalarVal();
	$volume = $m->getParam(1);
	$volume = $volume->scalarVal();
	$desc = $m->getParam(2);
	$desc = $desc->scalarVal();

	if (!is_array($config['samba']['share']))
		$config['samba']['share'] = array();

	array_sort_key($config['samba']['share'], "name");
	
	$a_samba = &$config['samba']['share'];

	if(false !== ($index = array_search_ex($sharename, $a_samba, "name")))
	{
		$returnmsg = Nas2BMsg("fail","Repeat samba name!");
		return new xmlrpcresp(php_xmlrpc_encode($returnmsg));
	}

	if (!is_array($config['volumes']['volume']))
		$config['volumes']['volume'] = array();
		
	array_sort_key($config['volumes']['volume'], "volumename");

	$a_volume = $config['volumes']['volume'];

	if(false === ($index = array_search_ex($volume, $a_volume, "volumename")))
	{
		$returnmsg = Nas2BMsg("fail","Wrong volume name!");
		return new xmlrpcresp(php_xmlrpc_encode($returnmsg));
	}
	else if($a_volume[$index]['filesystem'] !== "SHARE")
	{
		$returnmsg = Nas2BMsg("fail","Wrong volume name!");
		return new xmlrpcresp(php_xmlrpc_encode($returnmsg));		
	}

	$share = array();
	$retval = 0;
	
	$share['uuid'] = uuid();
	$share['name'] = $sharename;
	$share['volumemember'] = $volume;
	$share['path'] = "/mnt/".$volume."/";
	$share['comment'] = $desc;
	$share['readonly'] = false;
	$share['browseable'] = true;
	$share['inheritpermissions'] = true;
	$share['recyclebin'] = false;
	$share['hidedotfiles'] = true;
	$share['hostsallow'] = "";
	$share['hostsdeny'] = "";

	$a_samba[] = $share;

	write_config();

	config_lock();
	$retval |= rc_update_service("samba");
	$retval |= rc_update_service("mdnsresponder");
	config_unlock();

	if(!$retval)
	{
		$returnmsg = Nas2BMsg("success","Win share add success!");
	}
	else
	{
		$returnmsg = Nas2BMsg("fail","Win share add success!");
	}

	return new xmlrpcresp(php_xmlrpc_encode($returnmsg));
}

/************************************
name:	NasWinShareEdit
des:	win share edit process 
para:	
return:	
auth:	xiemian
date:	20111010
************************************/
function NasWinShareEdit($m)
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
	$sharename = $m->getParam(1);
	$sharename = $sharename->scalarVal();
	$volume = $m->getParam(2);
	$volume = $volume->scalarVal();
	$desc = $m->getParam(3);
	$desc = $desc->scalarVal();

	if (!is_array($config['samba']['share']))
		$config['samba']['share'] = array();

	array_sort_key($config['samba']['share'], "name");
	
	$a_samba = &$config['samba']['share'];

	if(false === ($index = array_search_ex($uuid, $a_samba, "uuid")))
	{
		$returnmsg = Nas2BMsg("fail","Win share id error!");
		return new xmlrpcresp(php_xmlrpc_encode($returnmsg));
	}
	
	if (!is_array($config['volumes']['volume']))
		$config['volumes']['volume'] = array();
		
	array_sort_key($config['volumes']['volume'], "volumename");

	$a_volume = $config['volumes']['volume'];

	if(false === ($cnid = array_search_ex($volume, $a_volume, "volumename")))
	{
		$returnmsg = Nas2BMsg("fail","Wrong volume name!");
		return new xmlrpcresp(php_xmlrpc_encode($returnmsg));
	}
	else if($a_volume[$cnid]['filesystem'] !== "SHARE")
	{
		$returnmsg = Nas2BMsg("fail","Wrong volume name!");
		return new xmlrpcresp(php_xmlrpc_encode($returnmsg));		
	}

	$retval = 0;

	$a_samba[$index]['name'] = $sharename;
	$a_samba[$index]['volumemember'] = $volume;
	$a_samba[$index]['path'] = "/mnt/".$volume."/";
	$a_samba[$index]['comment'] = $desc;

	write_config();

	config_lock();
	$retval |= rc_update_service("samba");
	$retval |= rc_update_service("mdnsresponder");
	config_unlock();

	if(!$retval)
	{
		$returnmsg = Nas2BMsg("success","Win share edit success!");
	}
	else
	{
		$returnmsg = Nas2BMsg("fail","Win share edit success!");
	}

	return new xmlrpcresp(php_xmlrpc_encode($returnmsg));
}

/************************************
name:	NasWinShareDel
des:	win share del process 
para:	
return:	
auth:	xiemian
date:	20111010
************************************/
function NasWinShareDel($m)
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

	if (!is_array($config['samba']['share']))
		$config['samba']['share'] = array();

	array_sort_key($config['samba']['share'], "name");
	
	$a_samba = &$config['samba']['share'];

	if(false === ($index = array_search_ex($uuid, $a_samba, "uuid")))
	{
		$returnmsg = Nas2BMsg("fail","Win share id error!");
		return new xmlrpcresp(php_xmlrpc_encode($returnmsg));
	}

	unset($a_samba[$index]);

	write_config();

	config_lock();
	$retval |= rc_update_service("samba");
	$retval |= rc_update_service("mdnsresponder");
	config_unlock();

	if(!$retval)
	{
		$returnmsg = Nas2BMsg("success","Win share del success!");
	}
	else
	{
		$returnmsg = Nas2BMsg("fail","Win share edit success!");
	}

	return new xmlrpcresp(php_xmlrpc_encode($returnmsg));
	
}

/************************************
name:	NasUnixShareList
des:	unix share list process 
para:	
return:	
auth:	xiemian
date:	20111010
************************************/
function NasUnixShareList($m)
{
	global $config;
	
	$share = array();
	$i = 0;

	if (!is_array($config['nfsd']['share']))
		$config['nfsd']['share'] = array();

	array_sort_key($config['nfsd']['share'], "path");
	
	$a_share = &$config['nfsd']['share'];
		
	foreach($a_share as $sharev)
	{
		$i++;
		$share['num'] = $i;
		$share['unixshare']['unixshare'.$i]['id'] = $sharev['uuid'];
		$share['unixshare']['unixshare'.$i]['sharename'] = $sharev['name'];
		$share['unixshare']['unixshare'.$i]['volume'] = $sharev['volumemember'];
		$share['unixshare']['unixshare'.$i]['network'] = $sharev['network'];
		$share['unixshare']['unixshare'.$i]['desc'] = $sharev['comment'];
	}
	
	$returnmsg = php_xmlrpc_encode($share);
	return new xmlrpcresp($returnmsg);

}

/************************************
name:	NasUnixShareAdd
des:	unix share add process 
para:	
return:	
auth:	xiemian
date:	20111010
************************************/
function NasUnixShareAdd($m)
{
	global $g;
	
	$d_sysrebootreqd_path = $g['varrun_path'] . "/sysreboot.reqd";

	if (file_exists($d_sysrebootreqd_path))
	{
		$returnmsg = Nas2BMsg("fail","Please reboot NAS first!");		
		return new xmlrpcresp(php_xmlrpc_encode($returnmsg));		
	}

	global $config;

	$sharename = $m->getParam(0);
	$sharename = $sharename->scalarVal();
	$volume = $m->getParam(1);
	$volume = $volume->scalarVal();
	$network = $m->getParam(2);
	$network = $network->scalarVal();
	$mask = $m->getParam(3);
	$mask = $mask->scalarVal();	
	$desc = $m->getParam(4);
	$desc = $desc->scalarVal();

	if (!is_array($config['nfsd']['share']))
		$config['nfsd']['share'] = array();

	array_sort_key($config['nfsd']['share'], "path");
	
	$a_nfsd = &$config['nfsd']['share'];

	if(!is_ipv4addr($network))
	{
		$returnmsg = Nas2BMsg("fail","Invalid IP address!");
		return new xmlrpcresp(php_xmlrpc_encode($returnmsg));
	}

	if(($mask != "8") && ($mask != "16") && ($mask != "24") && ($mask != "32"))
	{
		$returnmsg = Nas2BMsg("fail","Error mask!");
		return new xmlrpcresp(php_xmlrpc_encode($returnmsg));
	}

	if(false !== ($index = array_search_ex($sharename, $a_nfsd, "name")))
	{
		$returnmsg = Nas2BMsg("fail","Repeat nfs name!");
		return new xmlrpcresp(php_xmlrpc_encode($returnmsg));
	}

	if (!is_array($config['volumes']['volume']))
		$config['volumes']['volume'] = array();
		
	array_sort_key($config['volumes']['volume'], "volumename");

	$a_volume = $config['volumes']['volume'];

	if(false === ($index = array_search_ex($volume, $a_volume, "volumename")))
	{
		$returnmsg = Nas2BMsg("fail","Wrong volume name!");
		return new xmlrpcresp(php_xmlrpc_encode($returnmsg));
	}
	else if($a_volume[$index]['filesystem'] !== "SHARE")
	{
		$returnmsg = Nas2BMsg("fail","Wrong volume name!");
		return new xmlrpcresp(php_xmlrpc_encode($returnmsg));		
	}

	$share = array();
	$retval = 0;
	
	$share['uuid'] = uuid();
	$share['name'] = $sharename;
	$share['volumemember'] = $volume;
	$share['path'] = "/mnt/".$volume;
	$share['mapall'] = "yes";
	$share['network'] = gen_subnet($network,$mask)."/".$mask;
	$share['comment'] = $desc;
	$share['options'] = "";
	
	$a_nfsd[] = $share;
	write_config();

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
		$returnmsg = Nas2BMsg("success","Unix share add success!");
	}
	else
	{
		$returnmsg = Nas2BMsg("fail","Unix share add success!");
	}

	return new xmlrpcresp(php_xmlrpc_encode($returnmsg));

}

/************************************
name:	NasUnixShareEdit
des:	unix share edit process 
para:	
return:	
auth:	xiemian
date:	20111010
************************************/
function NasUnixShareEdit($m)
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
	$sharename = $m->getParam(1);
	$sharename = $sharename->scalarVal();
	$volume = $m->getParam(2);
	$volume = $volume->scalarVal();
	$network = $m->getParam(3);
	$network = $network->scalarVal();
	$mask = $m->getParam(4);
	$mask = $mask->scalarVal();
	$desc = $m->getParam(5);
	$desc = $desc->scalarVal();

	if(!is_ipv4addr($network))
	{
		$returnmsg = Nas2BMsg("fail","Invalid IP address!");
		return new xmlrpcresp(php_xmlrpc_encode($returnmsg));
	}

	if(($mask != "8") && ($mask != "16") && ($mask != "24") && ($mask != "32"))
	{
		$returnmsg = Nas2BMsg("fail","Error mask!");
		return new xmlrpcresp(php_xmlrpc_encode($returnmsg));
	}

	if (!is_array($config['nfsd']['share']))
		$config['nfsd']['share'] = array();

	array_sort_key($config['nfsd']['share'], "path");
	
	$a_nfsd = &$config['nfsd']['share'];

	if(false === ($index = array_search_ex($uuid, $a_nfsd, "uuid")))
	{
		$returnmsg = Nas2BMsg("fail","Unix share id error!");
		return new xmlrpcresp(php_xmlrpc_encode($returnmsg));
	}
	
	if (!is_array($config['volumes']['volume']))
		$config['volumes']['volume'] = array();
		
	array_sort_key($config['volumes']['volume'], "volumename");

	$a_volume = $config['volumes']['volume'];

	if(false === ($cnid = array_search_ex($volume, $a_volume, "volumename")))
	{
		$returnmsg = Nas2BMsg("fail","Wrong volume name!");
		return new xmlrpcresp(php_xmlrpc_encode($returnmsg));
	}
	else if($a_volume[$cnid]['filesystem'] !== "SHARE")
	{
		$returnmsg = Nas2BMsg("fail","Wrong volume name!");
		return new xmlrpcresp(php_xmlrpc_encode($returnmsg));		
	}

	$retval = 0;

	$a_nfsd[$index]['name'] = $sharename;
	$a_nfsd[$index]['volumemember'] = $volume;
	$a_nfsd[$index]['path'] = "/mnt/".$volume;
	$a_nfsd[$index]['network'] = gen_subnet($network,$mask)."/".$mask;
	$a_nfsd[$index]['comment'] = $desc;

	write_config();

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
		$returnmsg = Nas2BMsg("success","Unix share edit success!");
	}
	else
	{
		$returnmsg = Nas2BMsg("fail","Unix share edit success!");
	}

	return new xmlrpcresp(php_xmlrpc_encode($returnmsg));
}

/************************************
name:	NasUnixShareDel
des:	unix share del process 
para:	
return:	
auth:	xiemian
date:	20111010
************************************/
function NasUnixShareDel($m)
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

	if (!is_array($config['nfsd']['share']))
		$config['nfsd']['share'] = array();

	array_sort_key($config['nfsd']['share'], "path");
	
	$a_nfsd = &$config['nfsd']['share'];

	if(false === ($index = array_search_ex($uuid, $a_nfsd, "uuid")))
	{
		$returnmsg = Nas2BMsg("fail","Unix share id error!");
		return new xmlrpcresp(php_xmlrpc_encode($returnmsg));
	}

	unset($a_nfsd[$index]);

	write_config();

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
		$returnmsg = Nas2BMsg("success","Unix share del success!");
	}
	else
	{
		$returnmsg = Nas2BMsg("fail","Unix share edit success!");
	}

	return new xmlrpcresp(php_xmlrpc_encode($returnmsg));

}
?>
