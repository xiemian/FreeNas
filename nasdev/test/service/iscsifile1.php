#!/usr/local/bin/php
<?php
/*
	xmlrpcserver.php
	Copyright by xiemian
*/

require_once("config.inc");
require_once("util.inc");
require_once("system.inc");
require_once("XMLRPC/xmlrpc.inc");
require_once("XMLRPC/xmlrpcs.inc");


$msg = new xmlrpcmsg("iscsi.add");
$msg->addParam(new xmlrpcval("iscsifile","string"));
$msg->addParam(new xmlrpcval("volfile","string"));
$msg->addParam(new xmlrpcval("100","int"));
$msg->addParam(new xmlrpcval("xiemian","string"));


/*建立客户端*/
$client = new xmlrpc_client("/xmlrpcserver.php","192.168.10.251",80);
//$client->setDebug(1);

//echo "send msg\n";
$response = $client->send($msg);

$phpval = php_xmlrpc_decode($response->value());

print_r($phpval);

if($response->faultCode())
	echo "KO---".$response->faultString()."\n";
else
	echo "OK\n";




?>
