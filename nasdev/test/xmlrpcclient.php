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

/*����ʾ*/
//$msg = new xmlrpcmsg("group.list");

/*�����*/
/*
$msg = new xmlrpcmsg("group.add");
$msg->addParam(new xmlrpcval("testname111","string"));
$msg->addParam(new xmlrpcval("test desc","string"));
*/

/*���޸�*/
/*
$msg = new xmlrpcmsg("group.edit");
$msg->addParam(new xmlrpcval("3bf02ca3-c002-4015-88a0-11dd98e5469a","string"));
$msg->addParam(new xmlrpcval("test desc112121212121212","string"));
*/

/*��ɾ��*/
/*
$msg = new xmlrpcmsg("group.del");
$msg->addParam(new xmlrpcval("3bf02ca3-c002-4015-88a0-11dd98e5469a","string"));
*/

/*�û���ʾ*/
//$msg = new xmlrpcmsg("user.list");

/*�û����*/
/*
$msg = new xmlrpcmsg("user.add");
$msg->addParam(new xmlrpcval("1699","int"));
$msg->addParam(new xmlrpcval("guojian2","string"));
$msg->addParam(new xmlrpcval("1234567890123","string"));
$msg->addParam(new xmlrpcval("it","string"));
*/

/*�û��޸�*/
/*
$msg = new xmlrpcmsg("user.edit");
$msg->addParam(new xmlrpcval("a7aa801b-baea-4fc3-a2df-126910ec7f0f","string"));
$msg->addParam(new xmlrpcval("1018","int"));
$msg->addParam(new xmlrpcval("123456654321","string"));
$msg->addParam(new xmlrpcval("123","string"));
*/

/*�û�ɾ��*/
/*
$msg = new xmlrpcmsg("user.del");
$msg->addParam(new xmlrpcval("a7aa801b-baea-4fc3-a2df-126910ec7f0f","string"));
*/

/*������ʾ*/
//$msg = new xmlrpcmsg("volume.list");

/*������г�Ա��ʾ*/
//$msg = new xmlrpcmsg("volumemember.list");

/*�������*/
/*
$msg = new xmlrpcmsg("volume.add");
$msg->addParam(new xmlrpcval("vol3","string"));
$msg->addParam(new xmlrpcval("da1","string"));
$msg->addParam(new xmlrpcval("SIMPLE","string"));
$msg->addParam(new xmlrpcval("xiemian","string"));
$msg->addParam(new xmlrpcval("it","string"));
$msg->addParam(new xmlrpcval("651","int"));
$msg->addParam(new xmlrpcval("SHARE","string"));
*/

/*����༭*/
/*
$msg = new xmlrpcmsg("volume.edit");
$msg->addParam(new xmlrpcval("e2f3dcf8-6fe1-4bf7-b844-2d1c39b59119","string"));
$msg->addParam(new xmlrpcval("xiemian","string"));
$msg->addParam(new xmlrpcval("it","string"));
$msg->addParam(new xmlrpcval("137","int"));
*/

/*����ɾ��*/
/*
$msg = new xmlrpcmsg("volume.del");
$msg->addParam(new xmlrpcval("7c48d5a7-99dc-4c79-8b25-9941ec1dd436","string"));
*/

/*win������ʾ*/
//$msg = new xmlrpcmsg("winshare.list");

/*������г�Ա��ʾ*/
//$msg = new xmlrpcmsg("sharemember.list");

/*win�������*/
/*
$msg = new xmlrpcmsg("winshare.add");
$msg->addParam(new xmlrpcval("vol3share","string"));
$msg->addParam(new xmlrpcval("vol3","string"));
$msg->addParam(new xmlrpcval("vin share","string"));
*/

/*win����༭*/
/*
$msg = new xmlrpcmsg("winshare.edit");
$msg->addParam(new xmlrpcval("40d3eb6d-9f66-44a7-8f92-cdb3afefa1ff","string"));
$msg->addParam(new xmlrpcval("vol3share","string"));
$msg->addParam(new xmlrpcval("vol3","string"));
$msg->addParam(new xmlrpcval("win share","string"));
*/

/*win����ɾ��*/
/*
$msg = new xmlrpcmsg("winshare.del");
$msg->addParam(new xmlrpcval("c283a991-a91c-47e1-8325-fa03331048a3","string"));
*/

/*unix������ʾ*/
$msg = new xmlrpcmsg("unixshare.list");

/*unix�������*/
/*
$msg = new xmlrpcmsg("unixshare.add");
$msg->addParam(new xmlrpcval("vol3share","string"));
$msg->addParam(new xmlrpcval("vol3","string"));
$msg->addParam(new xmlrpcval("192.168.1.119","string"));
$msg->addParam(new xmlrpcval("24","string"));
$msg->addParam(new xmlrpcval("unix share","string"));
*/

/*unix����༭*/
/*
$msg = new xmlrpcmsg("unixshare.edit");
$msg->addParam(new xmlrpcval("e152bf25-3eed-49ff-923b-1d17cba49692","string"));
$msg->addParam(new xmlrpcval("vol3unixshare","string"));
$msg->addParam(new xmlrpcval("vol3","string"));
$msg->addParam(new xmlrpcval("192.168.11.119","string"));
$msg->addParam(new xmlrpcval("8","string"));
$msg->addParam(new xmlrpcval("unix share","string"));
*/

/*unix����ɾ��*/
/*
$msg = new xmlrpcmsg("unixshare.del");
$msg->addParam(new xmlrpcval("e152bf25-3eed-49ff-923b-1d17cba49692","string"));
*/

/*SMB����*/
/*��ʾ*/
//$msg = new xmlrpcmsg("smb.list");

/*����*/
/*
$msg = new xmlrpcmsg("smb.edit");
$msg->addParam(new xmlrpcval("open","string"));
$msg->addParam(new xmlrpcval("anonymous","string"));
*/


/*NFS����*/
/*��ʾ*/
//$msg = new xmlrpcmsg("nfs.list");

/*����*/
/*
$msg = new xmlrpcmsg("nfs.edit");
$msg->addParam(new xmlrpcval("open","string"));
$msg->addParam(new xmlrpcval("5","int"));
*/

/*FTP����*/
/*��ʾ*/
//$msg = new xmlrpcmsg("ftp.list");

/*����*/
/*
$msg = new xmlrpcmsg("ftp.edit");
$msg->addParam(new xmlrpcval("close","string"));
$msg->addParam(new xmlrpcval("21","int"));
$msg->addParam(new xmlrpcval("7","int"));
$msg->addParam(new xmlrpcval("allow","string"));
$msg->addParam(new xmlrpcval("200","int"));
$msg->addParam(new xmlrpcval("10","int"));
*/

/*ISCSI����*/
/*��ʾ*/
//$msg = new xmlrpcmsg("iscsi.list");

/*����*/
/*
$msg = new xmlrpcmsg("iscsi.set");
$msg->addParam(new xmlrpcval("open","string"));
*/

/*debice���г�Ա�б�*/
//$msg = new xmlrpcmsg("iscsidevicemember.list");

/*file���г�Ա�б�*/
//$msg = new xmlrpcmsg("iscsifilemember.list");

/*ISCSI����*/
/*
$msg = new xmlrpcmsg("iscsi.add");
$msg->addParam(new xmlrpcval("iscsi2","string"));
$msg->addParam(new xmlrpcval("vol1","string"));
$msg->addParam(new xmlrpcval("100","int"));
$msg->addParam(new xmlrpcval("xiemian","string"));
*/

/*ISCSI�༭*/
/*
$msg = new xmlrpcmsg("iscsi.edit");//d9ad2628-dce9-40b4-8cdd-b97ec56eaba1
$msg->addParam(new xmlrpcval("6b86adce-37c2-45db-bf96-9dfd5dd44d87","string"));//
$msg->addParam(new xmlrpcval("vol1","string"));
$msg->addParam(new xmlrpcval("100","int"));
$msg->addParam(new xmlrpcval("ALL","string"));
*/

/*ISCSIɾ��*/
/*
$msg = new xmlrpcmsg("iscsi.del");
$msg->addParam(new xmlrpcval("66058be1-e3bc-4241-98a8-599c9f979019","string"));
*/

/*ϵͳ����*/

/*���Ժ�NAS���������*/
$msg = new xmlrpcmsg("nas.test");

/*ϵͳ״̬*/
//$msg = new xmlrpcmsg("status.list");

/*����״̬*/
//$msg = new xmlrpcmsg("service.list");

/*����/�ػ�*/
//$msg = new xmlrpcmsg("boot.set");
//$msg->addParam(new xmlrpcval("reboot","string"));

/*������ʾ*/
//$msg = new xmlrpcmsg("network.list");

/*��������*/
/*
$msg = new xmlrpcmsg("network.set");
$msg->addParam(new xmlrpcval("le0","string"));
$msg->addParam(new xmlrpcval("STATIC","string"));
$msg->addParam(new xmlrpcval("192.168.10.150","string"));
$msg->addParam(new xmlrpcval("24","string"));
$msg->addParam(new xmlrpcval("192.168.10.1","string"));
*/


/* ���԰���*/
//$msg = new xmlrpcmsg("volumemember.list");
//$msg = new xmlrpcmsg("volume.list");
//$msg = new xmlrpcmsg("test.function");
//$msg->addParam(new xmlrpcval("192.54.111.111","string"));


/*�����ͻ���*/
$client = new xmlrpc_client("/xmlrpcserver.php","192.168.10.150",121);
$client->setDebug(1);

//echo "send msg\n";
$response = $client->send($msg,5);

$phpval = php_xmlrpc_decode($response->value());

print_r($phpval);

if($response->faultCode())
	echo "KO---".$response->faultString()."\n";
else
	echo "OK\n";




?>
