<?php
/*
	NasXmlRpc.inc
	Copyright by xiemian
*/


/************************************
name:	Nas2BMsg
des:	return ErrMsg array
para:	$result,$des
return:	$ReturnMsg
auth:	xiemian
date:	20110922
************************************/
function Nas2BMsg($result,$des)
{
	$ReturnMsg = array("result" => $result,
			"des" => $des);
	
	return $ReturnMsg;
}


/************************************
name:	get_nextgroup_id
des:	return unused id
para:	
return:	$id
auth:	xiemian
date:	20110922
************************************/
function get_nextgroup_id()
{
	global $config;

	// Get next free user id.
	exec("/usr/sbin/pw groupnext", $output);
	$output = explode(":", $output[0]);
	$id = intval($output[0]);

	// Check if id is already in usage. If the user did not press the 'Apply'
	// button 'pw' did not recognize that there are already several new users
	// configured because the user db is not updated until 'Apply' is pressed.
	$a_group = $config['access']['group'];
	if (false !== array_search_ex(strval($id), $a_group, "id")) {
		do {
			$id++; // Increase id until a unused one is found.
		} while (false !== array_search_ex(strval($id), $a_group, "id"));
	}

	return $id;
}

/************************************
name:	cmp_tag
des:	cmp
para:	
return:	
auth:	xiemian
date:	20111010
************************************/
function cmp_tag($a, $b)
{
	if ($a['tag'] == $b['tag'])
		return 0;
	return ($a['tag'] > $b['tag']) ? 1 : -1;
}

/************************************
name:	NasTest
des:	test the connectiong between nas and computer node
para:	
return:	
auth:	xiemian
date:	20111010
************************************/
function NasTest()
{
	$returnmsg = Nas2BMsg("success","Good connectiong!");
	return new xmlrpcresp(php_xmlrpc_encode($returnmsg));
}

/************************************
name:	Test
des:	test 
para:	
return:	
auth:	xiemian
date:	20111010
************************************/
function TestFunction($m)
{
	$ip = $m->getParam(0);
	$ip = $ip->scalarVal();

	if(is_ipv4addr($ip))
		$returnmsg = "ipv4---$ip";
	else
		$returnmsg = "ipv4!!!$ip";
		
	return new xmlrpcresp(php_xmlrpc_encode($returnmsg));
}

?>
