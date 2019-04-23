<?php
/*
	NasAccess.inc
	Copyright by xiemian
*/

/************************************
name:	NasAccessGroupList
des:	group list process 
para:	
return:	
auth:	xiemian
date:	20110922
************************************/
function NasAccessGroupList()
{
	global $config;
	
	$a_group = array();
	$i = 0;

	if (!is_array($config['access']['group']))
		$config['access']['group'] = array();

	array_sort_key($config['access']['group'], "name");
	
	$a_group_conf = &$config['access']['group'];
		
	foreach($a_group_conf as $groupv)
	{
		$i++;
		$a_group['num'] = $i;
		$a_group['group']['group'.$i]['id'] = $groupv['uuid'];
		$a_group['group']['group'.$i]['groupname'] = $groupv['name'];	
		$a_group['group']['group'.$i]['groupdesc'] = $groupv['desc'];
	}
	
	$returnmsg = php_xmlrpc_encode($a_group);
	return new xmlrpcresp($returnmsg);
}

/************************************
name:	NasAccessGroupAdd
des:	group add process 
para:	$m
return:	
auth:	xiemian
date:	20110922
************************************/
function NasAccessGroupAdd($m)
{
	global $g;
	
	$d_sysrebootreqd_path = $g['varrun_path'] . "/sysreboot.reqd";

	if (file_exists($d_sysrebootreqd_path))
	{
		$returnmsg = Nas2BMsg("fail","Please reboot NAS first!");		
		return new xmlrpcresp(php_xmlrpc_encode($returnmsg));		
	}

	global $config;
	
	$groupname = $m->getParam(0);
	$groupname = $groupname->scalarVal();
	$groupdesc = $m->getParam(1);
	$groupdesc = $groupdesc->scalarVal();

	if(!is_domain($groupname))
	{
		$returnmsg = Nas2BMsg("fail","Invalid group name!");
		return new xmlrpcresp(php_xmlrpc_encode($returnmsg));
	}

	if(!is_validdesc($groupdesc))
	{
		$returnmsg = Nas2BMsg("fail","Invalid group description!");
		return new xmlrpcresp(php_xmlrpc_encode($returnmsg));
	}

	if (!is_array($config['access']['group']))
		$config['access']['group'] = array();

	array_sort_key($config['access']['group'], "name");

	$a_group = &$config['access']['group'];
	$a_group_system = system_get_group_list();


	// Check for name conflicts. Only check if group is created.
	if ((false !== array_search_ex($groupname,$a_group,"name"))
	|| (is_array($a_group_system) && array_key_exists($groupname, $a_group_system))) 
	{
		$returnmsg = Nas2BMsg("fail","Repeated group name!");
		return new xmlrpcresp(php_xmlrpc_encode($returnmsg));
	}


	$groups['uuid'] = uuid();
	$groups['id'] = get_nextgroup_id();
	$groups['name'] = $groupname;
	$groups['desc'] = $groupdesc;
	
	$a_group[] = $groups;
	write_config();

	$retval = 0;
	
	config_lock();
	$retval |= rc_exec_service("userdb");
	config_unlock();

	if(!$retval)
	{
		$returnmsg = Nas2BMsg("success","Group add success!");
	}
	else
	{
		$returnmsg = Nas2BMsg("fail","Group add fail!");
	}
	
	return new xmlrpcresp(php_xmlrpc_encode($returnmsg));

}

/************************************
name:	NasAccessGroupEdit
des:	group edit process 
para:	$m
return:	
auth:	xiemian
date:	20110922
************************************/
function NasAccessGroupEdit($m)
{
	global $g;
	
	$d_sysrebootreqd_path = $g['varrun_path'] . "/sysreboot.reqd";

	if (file_exists($d_sysrebootreqd_path))
	{
		$returnmsg = Nas2BMsg("fail","Please reboot NAS first!");		
		return new xmlrpcresp(php_xmlrpc_encode($returnmsg));		
	}

	global $config;
	
	$groupuuid = $m->getParam(0);
	$groupuuid = $groupuuid->scalarVal();
	$groupdesc = $m->getParam(1);
	$groupdesc = $groupdesc->scalarVal();

	if (!is_array($config['access']['group']))
		$config['access']['group'] = array();

	array_sort_key($config['access']['group'], "name");

	$a_group = &$config['access']['group'];
	$a_group_system = system_get_group_list();

	if (false === ($cnid = array_search_ex($groupuuid,$a_group,"uuid"))) 
	{
		$returnmsg = Nas2BMsg("fail","Group id error!");
		return new xmlrpcresp(php_xmlrpc_encode($returnmsg));
	}


	if(!is_validdesc($groupdesc))
	{
		$returnmsg = Nas2BMsg("fail","Invalid group description!");
		return new xmlrpcresp(php_xmlrpc_encode($returnmsg));
	}
	
	$a_group[$cnid]['desc'] = $groupdesc;

	write_config();

	$retval = 0;
	
	config_lock();
	$retval |= rc_exec_service("userdb");
	config_unlock();

	if(!$retval)
	{
		$returnmsg = Nas2BMsg("success","Group edit success!");
	}
	else
	{
		$returnmsg = Nas2BMsg("fail","Group edit fail!");
	}
	
	return new xmlrpcresp(php_xmlrpc_encode($returnmsg));
	
	
}

/************************************
name:	NasAccessGroupDel
des:	group del process 
para:	$m
return:	
auth:	xiemian
date:	20110922
************************************/
function NasAccessGroupDel($m)
{
	global $g;
	
	$d_sysrebootreqd_path = $g['varrun_path'] . "/sysreboot.reqd";

	if (file_exists($d_sysrebootreqd_path))
	{
		$returnmsg = Nas2BMsg("fail","Please reboot NAS first!");		
		return new xmlrpcresp(php_xmlrpc_encode($returnmsg));		
	}

	global $config;
	
	$groupuuid = $m->getParam(0);
	$groupuuid = $groupuuid->scalarVal();

	if (!is_array($config['access']['group']))
		$config['access']['group'] = array();

	array_sort_key($config['access']['group'], "name");

	$a_group = &$config['access']['group'];

	if (false === ($cnid = array_search_ex($groupuuid,$a_group,"uuid"))) 
	{
		$returnmsg = Nas2BMsg("fail","Group id error!");
		return new xmlrpcresp(php_xmlrpc_encode($returnmsg));
	}

	unset($a_group[$cnid]);
	write_config();

	$retval = 0;
	
	config_lock();
	$retval |= rc_exec_service("userdb");
	config_unlock();

	if(!$retval)
	{
		$returnmsg = Nas2BMsg("success","Group del success!");
	}
	else
	{
		$returnmsg = Nas2BMsg("fail","Group del fail!");
	}
	
	return new xmlrpcresp(php_xmlrpc_encode($returnmsg));
}

/************************************
name:	NasAccessUserList
des:	user list process 
para:	
return:	
auth:	xiemian
date:	20110922
************************************/
function NasAccessUserList()
{
	global $config;
	
	$a_user = array();
	$i = 0;

	if (!is_array($config['access']['user']))
		$config['access']['user'] = array();

	array_sort_key($config['access']['user'], "login");
	
	$a_user_conf = &$config['access']['user'];
	$a_group = &$config['access']['group'];	
	
	foreach($a_user_conf as $userv)
	{
		$i++;
		$a_user['num'] = $i;
		$a_user['user']['user'.$i]['id'] = $userv['uuid'];
		$a_user['user']['user'.$i]['userid'] = $userv['id'];		
		$a_user['user']['user'.$i]['username'] = $userv['login'];

		if (false !== ($cnid = array_search_ex($userv['primarygroup'],$a_group,"id"))) 
		{
			$a_user['user']['user'.$i]['group'] = $a_group[$cnid]['name'];
		}
		else
		{
			$a_user['user']['user'.$i]['group'] = "undef";
		}
		
	}
	
	$returnmsg = php_xmlrpc_encode($a_user);
	
	return new xmlrpcresp($returnmsg);
	
}

/************************************
name:	NasAccessUserAdd
des:	user add process 
para:	$m
return:	
auth:	xiemian
date:	20110922
************************************/
function NasAccessUserAdd($m)
{
	global $g;
	
	$d_sysrebootreqd_path = $g['varrun_path'] . "/sysreboot.reqd";

	if (file_exists($d_sysrebootreqd_path))
	{
		$returnmsg = Nas2BMsg("fail","Please reboot NAS first!");		
		return new xmlrpcresp(php_xmlrpc_encode($returnmsg));		
	}

	global $config;
	
	$userid = $m->getParam(0);
	$userid = $userid->scalarVal();
	$username = $m->getParam(1);
	$username = $username->scalarVal();
	$userpassword = $m->getParam(2);
	$userpassword = $userpassword->scalarVal();
	$group = $m->getParam(3);
	$group = $group->scalarVal();

	if(($userid < 1001) || ($userid > 65500))
	{
		$returnmsg = Nas2BMsg("fail","Invalid user id!(1001-65500)");
		return new xmlrpcresp(php_xmlrpc_encode($returnmsg));
	}

	if(!is_validlogin($username))
	{
		$returnmsg = Nas2BMsg("fail","Invalid user name!");
		return new xmlrpcresp(php_xmlrpc_encode($returnmsg));
	}

	if(strlen($userpassword) < 12)
	{
		$returnmsg = Nas2BMsg("fail","Error user password Lenth!(>12)");
		return new xmlrpcresp(php_xmlrpc_encode($returnmsg));
	}

	if(strlen($username) >= 16)
	{
		$returnmsg = Nas2BMsg("fail","Error user name Lenth!(<16)");
		return new xmlrpcresp(php_xmlrpc_encode($returnmsg));
	}
	
	if (!is_array($config['access']['user']))
		$config['access']['user'] = array();

	array_sort_key($config['access']['user'], "login");

	$a_user = &$config['access']['user'];
	$a_group = &$config['access']['group'];	
	$a_user_system = system_get_user_list();


	// Check for name conflicts. Only check if user is created.
	if ((false !== array_search_ex($username,$a_user,"login"))
	|| (is_array($a_user_system) && array_key_exists($username, $a_user_system))) 
	{
		$returnmsg = Nas2BMsg("fail","Repeated user name!");
		return new xmlrpcresp(php_xmlrpc_encode($returnmsg));
	}

	// Check for id conflicts. Only check if id is created.
	if (false !== array_search_ex($userid,$a_user,"id"))
	{
		$returnmsg = Nas2BMsg("fail","Repeated user id!");
		return new xmlrpcresp(php_xmlrpc_encode($returnmsg));
	}


	// Check for group conflicts. Only check if group is created.
	if (false === ($cnid = array_search_ex($group,$a_group,"name")))
	{
		$returnmsg = Nas2BMsg("fail","Group name error!");
		return new xmlrpcresp(php_xmlrpc_encode($returnmsg));
	}


	$user = array();
	$user['uuid'] = uuid();
	$user['login'] = $username;
	$user['fullname'] = "";
	$user['password'] = $userpassword;
	$user['shell'] = "nologin";
	$user['primarygroup'] = $a_group[$cnid]['id'];
	$user['homedir'] = "";
	$user['id'] = $userid;
	$user['userportal'] = FALSE;
	
	$a_user[] = $user;
	write_config();

	$retval = 0;
	
	config_lock();
	$retval |= rc_exec_service("userdb");
	if (isset($config['samba']['enable'])) {
		$retval |= rc_exec_service("passdb");
		$retval |= rc_update_service("samba");
	}
	
	config_unlock();

	if(!$retval)
	{
		$returnmsg = Nas2BMsg("success","User add success!");
	}
	else
	{
		$returnmsg = Nas2BMsg("fail","User add fail!");
	}
	
	return new xmlrpcresp(php_xmlrpc_encode($returnmsg));
}

/************************************
name:	NasAccessUserEdit
des:	user edit process 
para:	$m
return:	
auth:	xiemian
date:	20110922
************************************/
function NasAccessUserEdit($m)
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
	$userid = $m->getParam(1);
	$userid = $userid->scalarVal();
	$userpassword = $m->getParam(2);
	$userpassword = $userpassword->scalarVal();
	$group = $m->getParam(3);
	$group = $group->scalarVal();

	if (!is_array($config['access']['user']))
		$config['access']['user'] = array();

	array_sort_key($config['access']['user'], "login");

	$a_user = &$config['access']['user'];
	$a_group = &$config['access']['group'];	

	if (false === ($cnid = array_search_ex($uuid,$a_user,"uuid"))) 
	{
		$returnmsg = Nas2BMsg("fail","Id error!");
		return new xmlrpcresp(php_xmlrpc_encode($returnmsg));
	}

	if(($userid < 1001) || ($userid > 65500))
	{
		$returnmsg = Nas2BMsg("fail","Invalid user id!(1001-65500)");
		return new xmlrpcresp(php_xmlrpc_encode($returnmsg));
	}
	
	// Check for id conflicts. Only check if id is created.
	if (false !== array_search_ex($userid,$a_user,"id"))
	{
		$returnmsg = Nas2BMsg("fail","Repeated user id!");
		return new xmlrpcresp(php_xmlrpc_encode($returnmsg));
	}

	if(strlen($userpassword) < 12)
	{
		$returnmsg = Nas2BMsg("fail","Error user password Lenth!(>12)");
		return new xmlrpcresp(php_xmlrpc_encode($returnmsg));
	}

	// Check for group conflicts. Only check if group is created.
	if (false === ($gnid = array_search_ex($group,$a_group,"name")))
	{
		$returnmsg = Nas2BMsg("fail","Group name error!");
		return new xmlrpcresp(php_xmlrpc_encode($returnmsg));
	}

	$a_user[$cnid]['id'] = $userid;
	$a_user[$cnid]['password'] = $userpassword;
	$a_user[$cnid]['primarygroup'] = $a_group[$gnid]['id'];

	write_config();

	$retval = 0;
	
	config_lock();
	$retval |= rc_exec_service("userdb");
	if (isset($config['samba']['enable'])) {
		$retval |= rc_exec_service("passdb");
		$retval |= rc_update_service("samba");
	}
	
	config_unlock();

	if(!$retval)
	{
		$returnmsg = Nas2BMsg("success","User edit success!");
	}
	else
	{
		$returnmsg = Nas2BMsg("fail","User edit fail!");
	}
	
	return new xmlrpcresp(php_xmlrpc_encode($returnmsg));
}

/************************************
name:	NasAccessUserDel
des:	user del process 
para:	$m
return:	
auth:	xiemian
date:	20110922
************************************/
function NasAccessUserDel($m)
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

	if (!is_array($config['access']['user']))
		$config['access']['user'] = array();

	array_sort_key($config['access']['user'], "login");

	$a_user = &$config['access']['user'];

	if (false === ($cnid = array_search_ex($uuid,$a_user,"uuid"))) 
	{
		$returnmsg = Nas2BMsg("fail","User id error!");
		return new xmlrpcresp(php_xmlrpc_encode($returnmsg));
	}

	unset($a_user[$cnid]);
	write_config();

	$retval = 0;
	
	config_lock();
	$retval |= rc_exec_service("userdb");
	config_unlock();

	if(!$retval)
	{
		$returnmsg = Nas2BMsg("success","User del success!");
	}
	else
	{
		$returnmsg = Nas2BMsg("fail","User del fail!");
	}
	
	return new xmlrpcresp(php_xmlrpc_encode($returnmsg));
	
}

?>
