<?php
/*
	NasVolume.inc
	Copyright by xiemian
*/

/************************************
name:	NasVolumeList
des:	volume list process 
para:	
return:	
auth:	xiemian
date:	20111010
************************************/
function NasVolumeList()
{
	global $config;

	$volume = array();
	$i = 0;

	if (!is_array($config['volumes']['volume']))
		$config['volumes']['volume'] = array();

	array_sort_key($config['volumes']['volume'], "volumename");
	
	$a_volume = &$config['volumes']['volume'];

	foreach($a_volume as $volumev)
	{
		$i++;
		$volume['num'] = $i;
		$volume['volume']['volume'.$i]['id'] = $volumev['uuid'];
		$volume['volume']['volume'.$i]['volumename'] = $volumev['volumename'];
		$volume['volume']['volume'.$i]['volumesize'] = $volumev['volumesize'];
		$volume['volume']['volume'.$i]['volumetype'] = $volumev['volumetype'];
		$volume['volume']['volume'.$i]['member'] = $volumev['member'];
		$volume['volume']['volume'.$i]['filesystem'] = $volumev['filesystem'];
		$volume['volume']['volume'.$i]['status'] = $volumev['status'];
	}
	
	$returnmsg = php_xmlrpc_encode($volume);
	return new xmlrpcresp($returnmsg);
}


/************************************
name:	NasVolumeMemberList
des:	volume list process 
para:	
return:	
auth:	xiemian
date:	20111010
************************************/
function NasVolumeMemberList()
{
	global $config,$g;

	// Load the /etc/cfdevice file to find out on which disk the OS is installed.
	$cfdevice = trim(file_get_contents("{$g['etc_path']}/cfdevice"));
	$volume = array();
	$i = 0;
	$find = 0;

	$a_phy_disk = get_physical_disks_list();

	if (!is_array($config['volumes']['volume']))
		$config['volumes']['volume'] = array();

	array_sort_key($config['volumes']['volume'], "volumename");

	$a_volume = &$config['volumes']['volume'];

	foreach($a_phy_disk as $diskk => $diskv)
	{
		foreach($a_volume as $volumev)
		{
			$disks = explode(" ",$volumev['member']);
			if(in_array($diskk,$disks))
			{
				$find = 1;
				continue;
			}
		}

		if($find == 1)
		{
			$find = 0;
			continue;
		}
		
		if (preg_match("/".preg_quote($diskk, "/")."\D+/", $cfdevice)) 
			continue;
		$i++;
		$volume['volumemember'.$i] = $diskk.": ".$diskv['size']."(".$diskv['desc'].")";
	}
	
	$returnmsg = php_xmlrpc_encode($volume);
	return new xmlrpcresp($returnmsg);
}

/************************************
name:	VolumeAddSim
des:	volume simple  
para:	
return:	
auth:	xiemian
date:	20111010
************************************/
function VolumeAddSim($volumename,$members,$user,$group,$permission,$filesystem)
{
	global $config;

	if (!is_array($config['disks']['disk']))
		$config['disks']['disk'] = array();
	array_sort_key($config['disks']['disk'], "name");
	
	$a_disk = &$config['disks']['disk'];
	$a_group = &$config['access']['group'];
	$a_user = &$config['access']['user'];

	$retval = 0;
	$result = array();

	$a_phy_disk = get_physical_disks_list();
	$diskname = "/dev/".$members;
	$diskftype = "ufsgpt";

	$disk['uuid'] = uuid();
	$disk['name'] = $members;
	$disk['apm'] = "0";
	$disk['transfermode'] = "auto";
	$disk['harddiskstandby'] = "0";
	$disk['acoustic'] = "0";
	$disk['devicespecialfile'] = $diskname;
	$disk['smart']['enable'] = false;
	$disk['smart']['extraoptions'] = "";
	$disk['desc'] = $a_phy_disk[$members]['desc'];
	$disk['fstype'] = $diskftype;
	$disk['type'] = $a_phy_disk[$members]['type'];
	$disk['size'] = $a_phy_disk[$members]['size'];
	
	$a_disk[] = $disk;
	write_config();
	config_lock();
	$retval |= rc_update_service("ataidle");
	config_unlock();

	if($filesystem != "ISCSI DEVICE")
	{
		ob_start();
		disks_format($diskname,$diskftype,false,"8","", true);
		ob_end_clean();

		$mountuuid = uuid();
		$mount['uuid'] = $mountuuid;
		$mount['type'] = "disk";
		$mount['mdisk'] = $diskname;
		$mount['partition'] = "p1";
		$mount['devicespecialfile'] = $diskname."p1";
		$mount['fstype'] = "ufs";
		$mount['sharename'] = $volumename;
		$mount['readonly'] = false;
		$mount['fsck'] = true;
		$mount['desc'] = "";
		if($filesystem == "SHARE")
		{
			$mount['accessrestrictions']['owner'] = $user;
			$mount['accessrestrictions']['group'] = $group;
			$mount['accessrestrictions']['mode'] = $permission;
		}
		else
		{
			$mount['accessrestrictions']['owner'] = "root";
			$mount['accessrestrictions']['group'] = "admin";
			$mount['accessrestrictions']['mode'] = "0777";
		}
		if (!is_array($config['mounts']['mount']))
			$config['mounts']['mount'] = array();
		array_sort_key($config['mounts']['mount'], "sharename");

		$a_mount = &$config['mounts']['mount'];
		
		$a_mount[] = $mount;
		write_config();

		if(false !== ($index = array_search_ex($mountuuid,$a_mount,"uuid")))
			disks_mount($config['mounts']['mount'][$index]);
		
		config_lock();
		$retval |= rc_update_service("samba");
		//$retval |= rc_update_service("rsyncd");
		$retval |= rc_update_service("afpd");
		$retval |= rc_update_service("rpcbind"); // !!! Do
		$retval |= rc_update_service("mountd");  // !!! not
		$retval |= rc_update_service("nfsd");    // !!! change
		$retval |= rc_update_service("statd");   // !!! this
		$retval |= rc_update_service("lockd");   // !!! order
		config_unlock();

	}

	if (!is_array($config['volumes']['volume']))
		$config['volumes']['volume'] = array();
	array_sort_key($config['volumes']['volume'], "volumename");

	$a_volume = &$config['volumes']['volume'];

	$volume['uuid'] = uuid();
	$volume['volumename'] = $volumename;
	$volume['volumetype'] = "SIMPLE";
	$volume['member'] = $members;
	switch($filesystem)
	{
		case "SHARE":
			$volume['filesystem'] = "SHARE";
			break;
		case "ISCSI DEVICE":
			$volume['filesystem'] = "ISCSI DEVICE";
			break;
		case "ISCSI FILE":
			$volume['filesystem'] = "ISCSI FILE";
			break;
	}
	$volume['volumesize'] = $a_phy_disk[$members]['size'];

	if($retval)
	{
		$volume['status'] = "ERROR";
		$result = Nas2BMsg("fail","Simple add fail!");
	}
	else
	{
		$volume['status'] = "OK";
		switch($filesystem)
		{
			case "SHARE":
				$result = Nas2BMsg("sucess","Simple add share success!");
				break;
			case "ISCSI DEVICE":
				$result = Nas2BMsg("sucess","Simple add iscsi device success!");
				break;
			case "ISCSI FILE":
				$result = Nas2BMsg("sucess","Simple add iscsi file success!");
				break;
		}
	}

	$a_volume[] = $volume;
	write_config();
	
	return $result;	
}

/************************************
name:	VolumeAddRaid
des:	volume jbod   
para:	
return:	
auth:	xiemian
date:	20111010
************************************/
function VolumeAddRaid($volumetype,$volumename,$members,$user,$group,$permission,$filesystem)
{
	global $config;

	$a_group = &$config['access']['group'];
	$a_user = &$config['access']['user'];

	$retval = 0;
	$result = array();
	$memberx = implode(" ",$members);
	
	$a_phy_disk = get_physical_disks_list();

	foreach($members as $memberk => $memberv)
	{
		if (!is_array($config['disks']['disk']))
			$config['disks']['disk'] = array();
		array_sort_key($config['disks']['disk'], "name");
		
		$a_disk = &$config['disks']['disk'];

		$diskname = "/dev/".$memberv;
		$diskftype = "softraid";
		
		$disk = array();
		$disk['uuid'] = uuid();
		$disk['name'] = $memberv;
		$disk['apm'] = "0";
		$disk['transfermode'] = "auto";
		$disk['harddiskstandby'] = "0";
		$disk['acoustic'] = "0";
		$disk['devicespecialfile'] = $diskname;
		$disk['smart']['enable'] = false;
		$disk['smart']['extraoptions'] = "";
		$disk['desc'] = $a_phy_disk[$memberv]['desc'];
		$disk['fstype'] = $diskftype;
		$disk['type'] = $a_phy_disk[$memberv]['type'];
		$disk['size'] = $a_phy_disk[$memberv]['size'];
		
		$a_disk[] = $disk;
		write_config();

		config_lock();
		$retval |= rc_update_service("ataidle");
		config_unlock();

		ob_start();
		disks_format($diskname,$diskftype,false,"8","", true);
		ob_end_clean();

	}
	
	$uuid = uuid();
	$graid['uuid'] = $uuid;
	$graid['name'] = $volumename;
	$graid['fstype'] = "ufsgpt";
	
	if($volumetype == "JBOD")
	{
		$type1 = "gconcat";
		$type2 = "concat";
		$graid['type'] = "JBOD";
	}
	if($volumetype == "RAID 0")
	{
		$type1 = "gstripe";
		$type2 = "stripe";
		$graid['type'] = "0";
	}
	if($volumetype == "RAID 1")
	{
		$type1 = "gmirror";
		$type2 = "mirror";
		$graid['type'] = "1";
		$graid['balance'] = "round-robin";
	}
	if($volumetype == "RAID 5")
	{
		$type1 = "graid5";
		$type2 = "raid5";
		$graid['type'] = "5";
	}
	
	if (!is_array($config[$type1]['vdisk']))
		$config[$type1]['vdisk'] = array();
	array_sort_key($config[$type1]['vdisk'], "name");

	$a_graid = &$config[$type1]['vdisk'];
	
	$graid['desc'] = "Software ".$type1." ".$volumetype;
	$graid['devicespecialfile'] = "/dev/".$type2."/{$volumename}";
	foreach($members as $memberk => $memberv)
	{
		$diskname = "/dev/".$memberv;
		$raiddevice[] = $diskname;
	}
	$graid['device'] = $raiddevice;
	$a_graid[] = $graid;
	write_config();
	
	$retval |= rc_exec_service("geom load ".$type2);
	
	if($volumetype == "JBOD")
	{
		$retval |= disks_raid_gconcat_configure($uuid);
	}
	if($volumetype == "RAID 0")
	{
		$retval |= disks_raid_gstripe_configure($uuid);
	}
	if($volumetype == "RAID 1")
	{
		$retval |= disks_raid_gmirror_configure($uuid);
	}
	if($volumetype == "RAID 5")
	{
		$retval |= rc_exec_service("geom tune ".$type2);
		$retval |= disks_raid_graid5_configure($uuid);
	}
	
	$diskname = "/dev/".$type2."/{$volumename}";
	if($filesystem != "ISCSI DEVICE")
	{
		$diskftype = "ufsgpt";
		ob_start();
		disks_format($diskname,$diskftype,false,"8","", true);
		ob_end_clean();
		
		$mountuuid = uuid();
		$mount['uuid'] = $mountuuid;
		$mount['type'] = "disk";
		$mount['mdisk'] = $diskname;
		$mount['partition'] = "p1";
		$mount['devicespecialfile'] = $diskname."p1";
		$mount['fstype'] = "ufs";
		$mount['sharename'] = $volumename;
		$mount['readonly'] = false;
		$mount['fsck'] = true;
		$mount['desc'] = "";
		if($filesystem == "SHARE")
		{
			$mount['accessrestrictions']['owner'] = $user;
			$mount['accessrestrictions']['group'] = $group;
			$mount['accessrestrictions']['mode'] = $permission;
		}
		else
		{
			$mount['accessrestrictions']['owner'] = "root";
			$mount['accessrestrictions']['group'] = "admin";
			$mount['accessrestrictions']['mode'] = "0777";
		}
		if (!is_array($config['mounts']['mount']))
			$config['mounts']['mount'] = array();
		array_sort_key($config['mounts']['mount'], "sharename");

		$a_mount = &$config['mounts']['mount'];
		
		$a_mount[] = $mount;
		write_config();

		if(false !== ($index = array_search_ex($mountuuid,$a_mount,"uuid")))
			disks_mount($config['mounts']['mount'][$index]);
		
		config_lock();
		$retval |= rc_update_service("samba");
		//$retval |= rc_update_service("rsyncd");
		$retval |= rc_update_service("afpd");
		$retval |= rc_update_service("rpcbind"); // !!! Do
		$retval |= rc_update_service("mountd");  // !!! not
		$retval |= rc_update_service("nfsd");    // !!! change
		$retval |= rc_update_service("statd");   // !!! this
		$retval |= rc_update_service("lockd");   // !!! order
		config_unlock();

	}
	
	if (!is_array($config['volumes']['volume']))
		$config['volumes']['volume'] = array();
	array_sort_key($config['volumes']['volume'], "volumename");

	$a_volume = &$config['volumes']['volume'];

	if($volumetype == "JBOD")
	{
		$raidstatus = get_gconcat_disks_list();
	}
	if($volumetype == "RAID 0")
	{
		$raidstatus = get_gstripe_disks_list();
	}
	if($volumetype == "RAID 1")
	{
		$raidstatus = get_gmirror_disks_list();
	}
	if($volumetype == "RAID 5")
	{
		$raidstatus = get_graid5_disks_list();
	}
	

	$volume['uuid'] = uuid();
	$volume['volumename'] = $volumename;	
	$volume['volumetype'] = $volumetype;	
	$volume['member'] = $memberx;
	switch($filesystem)
	{
		case "SHARE":
			$volume['filesystem'] = "SHARE";
			break;
		case "ISCSI DEVICE":
			$volume['filesystem'] = "ISCSI DEVICE";
			break;
		case "ISCSI FILE":
			$volume['filesystem'] = "ISCSI FILE";
			break;
	}
	$volume['volumesize'] = $raidstatus[$volumename]['size'];

	if($retval)
	{
		$volume['status'] = "ERROR";
		$result = Nas2BMsg("fail","Raid add fail!");
	}
	else
	{
		$volume['status'] = "OK";
		switch($filesystem)
		{
			case "SHARE":
				$result = Nas2BMsg("sucess",$volumetype." add share success!");
				break;
			case "ISCSI DEVICE":
				$result = Nas2BMsg("sucess",$volumetype." add iscsi device success!");
				break;
			case "ISCSI FILE":
				$result = Nas2BMsg("sucess",$volumetype." add iscsi file success!");
				break;
		}
	}
	
	$a_volume[] = $volume;
	write_config();

	return $result;		
}


/************************************
name:	NasVolumeAdd
des:	volume add process 
para:	$m
return:	
auth:	xiemian
date:	20111010
************************************/
function NasVolumeAdd($m)
{
	global $g;
	
	$d_sysrebootreqd_path = $g['varrun_path'] . "/sysreboot.reqd";

	if (file_exists($d_sysrebootreqd_path))
	{
		$returnmsg = Nas2BMsg("fail","Please reboot NAS first!");		
		return new xmlrpcresp(php_xmlrpc_encode($returnmsg));		
	}

	global $config;

	$volumename = $m->getParam(0);
	$volumename = $volumename->scalarVal();
	$member = $m->getParam(1);
	$member = $member->scalarVal();
	$volumetype = $m->getParam(2);
	$volumetype = $volumetype->scalarVal();
	$user = $m->getParam(3);
	$user = $user->scalarVal();
	$group = $m->getParam(4);
	$group = $group->scalarVal();
	$permission = $m->getParam(5);
	$permission = $permission->scalarVal();
	$filesystem = $m->getParam(6);
	$filesystem = $filesystem->scalarVal();

	if (!is_array($config['volumes']['volume']))
		$config['volumes']['volume'] = array();
	array_sort_key($config['volumes']['volume'], "volumename");

	$a_volume = &$config['volumes']['volume'];
	$a_disk = &$config['disks']['disk'];
	$a_user = &$config['access']['user'];
	$a_group = &$config['access']['group'];	

	if(!is_validaliasname($volumename))
	{
		$returnmsg = Nas2BMsg("fail","Invalid volume name!");
		return new xmlrpcresp(php_xmlrpc_encode($returnmsg));
	}

	$volumename = substr($volumename,0,15);

	$members = explode(" ",$member);
	foreach($members as $memberk => $memberv)
	{
		if (preg_match("/".preg_quote($memberv, "/")."\D+/", $cfdevice)) 
		{
			$returnmsg = Nas2BMsg("fail","Error member!");
			return new xmlrpcresp(php_xmlrpc_encode($returnmsg));			
		}

		if(false !== array_search_ex($memberv, $a_disk, "name"))
		{
			$returnmsg = Nas2BMsg("fail","Error member!");
			return new xmlrpcresp(php_xmlrpc_encode($returnmsg));			
		}
	}

	if(false !== array_search_ex($volumename,$a_volume,"volumename"))
	{
		$returnmsg = Nas2BMsg("fail","Repeat volumename!");
		return new xmlrpcresp(php_xmlrpc_encode($returnmsg));
	}

	if(($volumetype == "SIMPLE") || ($volumetype == "JBOD") 
	|| ($volumetype == "RAID 0") || ($volumetype == "RAID 1") || ($volumetype == "RAID 5"))
	{		
	}
	else
	{
		$returnmsg = Nas2BMsg("fail","Error type!");
		return new xmlrpcresp(php_xmlrpc_encode($returnmsg));		
	}

	if(($filesystem == "SHARE") || ($filesystem == "ISCSI DEVICE") || ($filesystem == "ISCSI FILE"))
	{
	}
	else
	{
		$returnmsg = Nas2BMsg("fail","Error file system!");
		return new xmlrpcresp(php_xmlrpc_encode($returnmsg));
	}

	if($volumetype != "ISCSI DEVICE")
	{
		if(false === array_search_ex($user,$a_user,"login"))
		{
			$returnmsg = Nas2BMsg("fail","Error user!");
			return new xmlrpcresp(php_xmlrpc_encode($returnmsg));
		}

		
		if(false === array_search_ex($group,$a_group,"name"))
		{
			$returnmsg = Nas2BMsg("fail","Error group!");
			return new xmlrpcresp(php_xmlrpc_encode($returnmsg));
		}

		if(octdec($permission) > 511)
		{
			$returnmsg = Nas2BMsg("fail","Error permission!");
			return new xmlrpcresp(php_xmlrpc_encode($returnmsg));
		}
	}

	if(($volumetype == "SIMPLE") && (count($members) != 1))
	{
		$returnmsg = Nas2BMsg("fail","Error member number!(must = 1)!");
		return new xmlrpcresp(php_xmlrpc_encode($returnmsg));
	}
	
	if(($volumetype == "JBOD") && (count($members) < 2))
	{
		$returnmsg = Nas2BMsg("fail","Error member number!(must >= 2)!");
		return new xmlrpcresp(php_xmlrpc_encode($returnmsg));
	}
	if(($volumetype == "RAID 0") && (count($members) < 2))
	{
		$returnmsg = Nas2BMsg("fail","Error member number!(must >= 2)!");
		return new xmlrpcresp(php_xmlrpc_encode($returnmsg));
	}

	if(($volumetype == "RAID 1") && (count($members) != 2))
	{
		$returnmsg = Nas2BMsg("fail","Error member number!(must = 2)!");
		return new xmlrpcresp(php_xmlrpc_encode($returnmsg));
	}
	
	if(($volumetype == "RAID 5") && (count($members) < 2))
	{
		$returnmsg = Nas2BMsg("fail","Error member number!(must >= 2)!");
		return new xmlrpcresp(php_xmlrpc_encode($returnmsg));
	}

	if($volumetype == "SIMPLE")
	{
		$returnmsg = VolumeAddSim($volumename,$members[0],$user,$group,$permission,$filesystem);
	}
	else
	{
		$returnmsg = VolumeAddRaid($volumetype,$volumename,$members,$user,$group,$permission,$filesystem);
	}

	return new xmlrpcresp(php_xmlrpc_encode($returnmsg));
}

/************************************
name:	NasVolumeEdit
des:	volume edit process 
para:	$m
return:	
auth:	xiemian
date:	20111010
************************************/
function NasVolumeEdit($m)
{
	global $g;
	
	$d_sysrebootreqd_path = $g['varrun_path'] . "/sysreboot.reqd";

	if (file_exists($d_sysrebootreqd_path))
	{
		$returnmsg = Nas2BMsg("fail","Please reboot NAS first!");		
		return new xmlrpcresp(php_xmlrpc_encode($returnmsg));		
	}

	global $config;
	
	$volumeuuid = $m->getParam(0);
	$volumeuuid = $volumeuuid->scalarVal();
	$user = $m->getParam(1);
	$user = $user->scalarVal();
	$group = $m->getParam(2);
	$group = $group->scalarVal();
	$permission = $m->getParam(3);
	$permission = $permission->scalarVal();

	$a_user = &$config['access']['user'];
	$a_group = &$config['access']['group'];	

	if(false === array_search_ex($user,$a_user,"login"))
	{
		$returnmsg = Nas2BMsg("fail","Error user!");
		return new xmlrpcresp(php_xmlrpc_encode($returnmsg));
	}

	
	if(false === array_search_ex($group,$a_group,"name"))
	{
		$returnmsg = Nas2BMsg("fail","Error group!");
		return new xmlrpcresp(php_xmlrpc_encode($returnmsg));
	}

	if(octdec($permission) > 511)
	{
		$returnmsg = Nas2BMsg("fail","Error permission!");
		return new xmlrpcresp(php_xmlrpc_encode($returnmsg));
	}

	if (!is_array($config['volumes']['volume']))
		$config['volumes']['volume'] = array();

	array_sort_key($config['volumes']['volume'], "volumename");

	$a_volume = &$config['volumes']['volume'];

	if (false === ($cnid = array_search_ex($volumeuuid,$a_volume,"uuid"))) 
	{
		$returnmsg = Nas2BMsg("fail","Volume id error!");
		return new xmlrpcresp(php_xmlrpc_encode($returnmsg));
	}

	$volumename = $a_volume[$cnid]['volumename'];
	$filesystem = $a_volume[$cnid]['filesystem'];

	if($filesystem == "SHARE")
	{
	}
	else
	{
		$returnmsg = Nas2BMsg("fail","Error, ISCSI DEVICE/FILE cann't be modified!");
		return new xmlrpcresp(php_xmlrpc_encode($returnmsg));
	}

	if (!is_array($config['mounts']['mount']))
		$config['mounts']['mount'] = array();
	array_sort_key($config['mounts']['mount'], "sharename");

	$a_mount = &$config['mounts']['mount'];

	if(false !== ($index = array_search_ex($volumename,$a_mount,"sharename")))
	{
		$a_mount[$index]['accessrestrictions']['owner'] = $user;
		$a_mount[$index]['accessrestrictions']['group'] = $group;
		$a_mount[$index]['accessrestrictions']['mode'] = $permission;		
	}
	else
	{
		$returnmsg = Nas2BMsg("fail","Error volume!");
		return new xmlrpcresp(php_xmlrpc_encode($returnmsg));	
	}

	write_config();

	config_lock();
	$retval |= rc_update_service("samba");
	$retval |= rc_update_service("rsyncd");
	$retval |= rc_update_service("afpd");
	$retval |= rc_update_service("rpcbind"); // !!! Do
	$retval |= rc_update_service("mountd");  // !!! not
	$retval |= rc_update_service("nfsd");    // !!! change
	$retval |= rc_update_service("statd");   // !!! this
	$retval |= rc_update_service("lockd");   // !!! order
	config_unlock();

	if(!$retval)
	{
		$returnmsg = Nas2BMsg("success","Volume edit success!");
	}
	else
	{
		$returnmsg = Nas2BMsg("fail","Volume edit fail!");
	}

	return new xmlrpcresp(php_xmlrpc_encode($returnmsg));
}


/************************************
name:	VolumeDelSim
des:	volume simple  
para:	
return:	
auth:	xiemian
date:	20111010
************************************/
function VolumeDelSim($volumename,$members,$filesystem)
{
	global $config;

	$retval = 0;
	
	if($filesystem != "ISCSI DEVICE")
	{
		if (!is_array($config['mounts']['mount']))
			$config['mounts']['mount'] = array();
		array_sort_key($config['mounts']['mount'], "sharename");

		$a_mount = &$config['mounts']['mount'];
		
		if(false !== ($index = array_search_ex($volumename,$a_mount,"sharename")))
		{
			disks_umount($config['mounts']['mount'][$index]);
			unset($config['mounts']['mount'][$index]);
			write_config();

			config_lock();
			$retval |= rc_update_service("samba");
			$retval |= rc_update_service("rsyncd");
			$retval |= rc_update_service("afpd");
			$retval |= rc_update_service("rpcbind"); // !!! Do
			$retval |= rc_update_service("mountd");  // !!! not
			$retval |= rc_update_service("nfsd");    // !!! change
			$retval |= rc_update_service("statd");   // !!! this
			$retval |= rc_update_service("lockd");   // !!! order
			config_unlock();
		}
	}

	if (is_array($config['disks']['disk']))
	{
		$index = array_search_ex($members, $config['disks']['disk'], "name");
		if (false !== $index)
		{
			unset($config['disks']['disk'][$index]);
			write_config();
		}
	}

	config_lock();
	$retval |= rc_update_service("ataidle");
	config_unlock();

	if(!$retval)
	{
		$result = Nas2BMsg("success","Volume del success!");
	}
	else
	{
		$result = Nas2BMsg("fail","Volume del fail!");
	}

	return $result;
}

/************************************
name:	VolumeDelRaid
des:	volume raid  
para:	
return:	
auth:	xiemian
date:	20111010
************************************/
function VolumeDelRaid($volumetype,$volumename,$members,$filesystem)
{
	global $config;

	$retval = 0;
	
	if($filesystem != "ISCSI DEVICE")
	{
		if (!is_array($config['mounts']['mount']))
			$config['mounts']['mount'] = array();
		array_sort_key($config['mounts']['mount'], "sharename");

		$a_mount = &$config['mounts']['mount'];
		
		if(false !== ($index = array_search_ex($volumename,$a_mount,"sharename")))
		{
			disks_umount($config['mounts']['mount'][$index]);
			unset($config['mounts']['mount'][$index]);
			write_config();

			config_lock();
			$retval |= rc_update_service("samba");
			$retval |= rc_update_service("rsyncd");
			$retval |= rc_update_service("afpd");
			$retval |= rc_update_service("rpcbind"); // !!! Do
			$retval |= rc_update_service("mountd");  // !!! not
			$retval |= rc_update_service("nfsd");    // !!! change
			$retval |= rc_update_service("statd");   // !!! this
			$retval |= rc_update_service("lockd");   // !!! order
			config_unlock();
		}
	}

	if($volumetype == "JBOD")
	{
		$type1 = "gconcat";
	}
	if($volumetype == "RAID 0")
	{
		$type1 = "gstripe";
	}
	if($volumetype == "RAID 1")
	{
		$type1 = "gmirror";
	}
	if($volumetype == "RAID 5")
	{
		$type1 = "graid5";
	}

	if (is_array($config[$type1]['vdisk']))
	{
		$index = array_search_ex($volumename, $config[$type1]['vdisk'], "name");

		if (false !== $index)
		{
			$uuid = $config[$type1]['vdisk'][$index]['uuid'];
			
			if($volumetype == "JBOD")
			{
				$retval |= disks_raid_gconcat_delete($uuid);
			}
			if($volumetype == "RAID 0")
			{
				$retval |= disks_raid_gstripe_delete($uuid);
			}
			if($volumetype == "RAID 1")
			{
				$retval |= disks_raid_gmirror_delete($uuid);
			}
			if($volumetype == "RAID 5")
			{
				$retval |= disks_raid_graid5_delete($uuid);
			}
			
			unset($config[$type1]['vdisk'][$index]);
			write_config();
		}
	}

	foreach($members as $memberk => $memberv)
	{
		if (is_array($config['disks']['disk']))
		{
			$index = array_search_ex($memberv, $config['disks']['disk'], "name");
			if (false !== $index)
			{
				unset($config['disks']['disk'][$index]);
				write_config();
			}
		}
	}
	
	config_lock();
	$retval |= rc_update_service("ataidle");
	config_unlock();

	if(!$retval)
	{
		$result = Nas2BMsg("success","Volume del success!");
	}
	else
	{
		$result = Nas2BMsg("fail","Volume del fail!");
	}

	return $result;
}

/************************************
name:	NasVolumeDel
des:	volume del process 
para:	$m
return:	
auth:	xiemian
date:	20111010
************************************/
function NasVolumeDel($m)
{
	global $g;
	
	$d_sysrebootreqd_path = $g['varrun_path'] . "/sysreboot.reqd";

	if (file_exists($d_sysrebootreqd_path))
	{
		$returnmsg = Nas2BMsg("fail","Please reboot NAS first!");		
		return new xmlrpcresp(php_xmlrpc_encode($returnmsg));		
	}

	global $config;

	$volumeuuid = $m->getParam(0);
	$volumeuuid = $volumeuuid->scalarVal();

	if (!is_array($config['volumes']['volume']))
		$config['volumes']['volume'] = array();

	array_sort_key($config['volumes']['volume'], "volumename");

	$a_volume = &$config['volumes']['volume'];

	if (false === ($cnid = array_search_ex($volumeuuid,$a_volume,"uuid"))) 
	{
		$returnmsg = Nas2BMsg("fail","Volume id error!");
		return new xmlrpcresp(php_xmlrpc_encode($returnmsg));
	}

	$volumename = $a_volume[$cnid]['volumename'];
	$volumetype = $a_volume[$cnid]['volumetype'];
	$member = $a_volume[$cnid]['member'];
	$filesystem = $a_volume[$cnid]['filesystem'];

	$members = explode(" ",$member);

	unset($a_volume[$cnid]);
	write_config();

	if($volumetype == "SIMPLE")
	{
		$returnmsg = VolumeDelSim($volumename,$members[0],$filesystem);
	}
	else
	{
		$returnmsg = VolumeDelRaid($volumetype,$volumename,$members,$filesystem);
	}
	
	return new xmlrpcresp(php_xmlrpc_encode($returnmsg));
}

?>
