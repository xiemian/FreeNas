<?php
/*
	NasSystem.inc
	Copyright by xiemian
*/

/************************************
name:	NasStatusList
des:	system status process 
para:	
return:	
auth:	xiemian
date:	20111010
************************************/
function NasStatusList()
{
	$systeminfo = array();

	$cpus = system_get_cpus();
	$cpuinfo = system_get_cpu_info();
	$cpuused = system_get_smp_cpu_usage();
	$j = 0;

	$systeminfo['cpu']['num'] = $cpus;
	$systeminfo['cpu']['frequence'] = $cpuinfo['freq'];
	for($i = 0;$i < $cpus;$i++)
	{
		$systeminfo['cpu']['cpu'.($i + 1)]['used'] = $cpuused[$i];
		$systeminfo['cpu']['cpu'.($i + 1)]['temp'] = $cpuinfo['temperature2'][$i];
	}

	$raminfo = system_get_ram_info();
	$systeminfo['memory']['used'] = $raminfo['used'];
	$systeminfo['memory']['total'] = $raminfo['total'];

	$diskusage = system_get_mount_usage();
	array_sort_key($diskusage, "name");
	$systeminfo['volume']['num'] = count($diskusage);
	foreach($diskusage as $diskusagek => $diskusagev)
	{
		$j++;
		$systeminfo['volume']['vol']['vol'.$j]['name'] = $diskusagev['name'];
		$systeminfo['volume']['vol']['vol'.$j]['total'] = $diskusagev['size'];
		$systeminfo['volume']['vol']['vol'.$j]['used'] = $diskusagev['used'];
	}

	return new xmlrpcresp(php_xmlrpc_encode($systeminfo));
}



/************************************
name:	NasServiceList
des:	service status process 
para:	
return:	
auth:	xiemian
date:	20111010
************************************/
function NasServiceList()
{
	global $config;

	$serviceinfo = array();

	$serviceinfo['num'] = 4;
		
	$serviceinfo['service']['smb'] = isset($config['samba']['enable'])?"open":"close";
	$serviceinfo['service']['nfs'] = isset($config['nfsd']['enable'])?"open":"close";
	$serviceinfo['service']['iscsi'] = isset($config['iscsitarget']['enable'])?"open":"close";
	$serviceinfo['service']['ftp'] = isset($config['ftpd']['enable'])?"open":"close";
	
	return new xmlrpcresp(php_xmlrpc_encode($serviceinfo));
}



/************************************
name:	NasBootSet
des:	reboot/shutdown 
para:	
return:	
auth:	xiemian
date:	20111010
************************************/
function NasBootSet($m)
{
	$act = $m->getParam(0);
	$act = $act->scalarVal();
	
	if($act === "reboot")
		system_reboot();
	else if($act === "shutdown")
		system_halt();
	else if($act === "factory")
	{
		reset_factory_defaults();
		system_reboot();
	}
	else
	{
		$returnmsg = Nas2BMsg("fail","Invalid action!");
		return new xmlrpcresp(php_xmlrpc_encode($returnmsg));
	}
}

/************************************
name:	NasNetworkList
des:	network list process 
para:	
return:	
auth:	xiemian
date:	20111010
************************************/
function NasNetworkList($m)
{
	global $config;

	/* get list without VLAN interfaces */
	$portlist = get_interface_list();

	// Add LAGG interfaces.
	if (is_array($config['vinterfaces']['lagg']) && count($config['vinterfaces']['lagg'])) {
		foreach ($config['vinterfaces']['lagg'] as $laggv) {
			$portlist[$laggv['if']] = $laggv;
			$portlist[$laggv['if']]['isvirtual'] = true;
		}
	}

	$a_net = &$config['interfaces']['lan'];

	$network = array();

	if($a_net['if'] == "lagg0")
		$network['mode'] = "LACP";
	else
		$network['mode'] = $a_net['if'];
		
	if($a_net['ipaddr'] == "dhcp")
	{
		$network['lan']['type'] = "DHCP";
		$network['lan']['ipaddress'] = get_ipaddr($network['mode']);
		$network['lan']['mask'] = get_subnet_bits($network['mode']);
	}
	else
	{
		$network['lan']['type'] = "STATIC";
		$network['lan']['ipaddress'] = $a_net['ipaddr'];
		$network['lan']['mask'] = $a_net['subnet'];
	}

	$network['lan']['gateway'] = get_defaultgateway();

	return new xmlrpcresp(php_xmlrpc_encode($network));
}

/************************************
name:	NasNetworkSet
des:	network Set process 
para:	
return:	
auth:	xiemian
date:	20111010
************************************/
function NasNetworkSet($m)
{
	global $g;
	
	$d_sysrebootreqd_path = $g['varrun_path'] . "/sysreboot.reqd";

	if (file_exists($d_sysrebootreqd_path))
	{
		$returnmsg = Nas2BMsg("fail","Please reboot NAS first!");		
		return new xmlrpcresp(php_xmlrpc_encode($returnmsg));		
	}

	global $config;

	$mode = $m->getParam(0);
	$mode = $mode->scalarVal();
	$type = $m->getParam(1);
	$type = $type->scalarVal();
	$ipaddress = $m->getParam(2);
	$ipaddress = $ipaddress->scalarVal();
	$mask = $m->getParam(3);
	$mask = $mask->scalarVal();
	$gateway = $m->getParam(4);
	$gateway = $gateway->scalarVal();

	/* get list without VLAN interfaces */
	$portlist = get_interface_list();
	$a_net = &$config['interfaces']['lan'];
	$a_vnet = &$config['vinterfaces']['lagg'];
	$flag = 0;
	
	if($mode != "LACP")
	{
		foreach($portlist as $portk => $portv)
		{
			if($portk == $mode)
			{
				$flag = 1;
				break;
			}
		}
		if(!$flag)
		{
			$returnmsg = Nas2BMsg("fail","Error mode!");
			return new xmlrpcresp(php_xmlrpc_encode($returnmsg));
		}
	}

	if(($type !== "STATIC") && ($type !== "DHCP"))
	{
		$returnmsg = Nas2BMsg("fail","Error type!");
		return new xmlrpcresp(php_xmlrpc_encode($returnmsg));
	}

	if(!is_ipv4addr($ipaddress))
	{
		$returnmsg = Nas2BMsg("fail","Invalid IP address!");
		return new xmlrpcresp(php_xmlrpc_encode($returnmsg));
	}

	if(($mask != "8") && ($mask != "16") && ($mask != "24") && ($mask != "32"))
	{
		$returnmsg = Nas2BMsg("fail","Error mask!");
		return new xmlrpcresp(php_xmlrpc_encode($returnmsg));
	}

	if($mode == "LACP")
	{
		if(!is_array($a_vnet))
		{
			$lagg = array();
			$lagg['enable'] = true;
			$lagg['uuid'] = uuid();
			$lagg['if'] = "lagg0";
			$lagg['laggproto'] = "lacp";//none xiemian 20111031
			foreach($portlist as $portk => $portv)
			{
				if($portk != "lagg0")
					$lagg['laggport'][] = $portk;
				else
					continue;
			}
			$lagg['desc'] = $_POST['desc'];

			$a_vnet[] = $lagg;
		}

		$a_net['if'] = "lagg0";
	}
	else
		$a_net['if'] = "$mode";

	if($type == "DHCP")
	{
		$a_net['ipaddr'] = "dhcp";
	}
	else
	{
		$a_net['ipaddr'] = $ipaddress;
		$a_net['subnet'] = $mask;
		$a_net['gateway'] = $gateway;
	}

	$a_net['mtu'] = "";

	write_config();
	
	ob_start();
	touch($d_sysrebootreqd_path);
	ob_end_clean();

	$returnmsg = Nas2BMsg("success","Network set success!");
	return new xmlrpcresp(php_xmlrpc_encode($returnmsg));
	
}



?>
