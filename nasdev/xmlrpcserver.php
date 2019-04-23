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

require_once("NasXmlRpc.inc");			/*xmlrpc util*/

require_once("NasAccess.inc");			/*access*/
require_once("NasVolume.inc");			/*volume*/	
require_once("NasShare.inc");			/*share*/
require_once("NasService.inc");			/*service*/
require_once("NasSystem.inc");			/*system*/

/*access signature*/
$grouplist_sig = array(array($xmlrpcStruct));
$groupadd_sig = array(array($xmlrpcStruct,$xmlrpcString,$xmlrpcString));
$groupedit_sig = array(array($xmlrpcStruct,$xmlrpcString,$xmlrpcString));
$groupdel_sig = array(array($xmlrpcStruct,$xmlrpcString));

$userlist_sig = array(array($xmlrpcStruct));
$useradd_sig = array(array($xmlrpcStruct,$xmlrpcInt,$xmlrpcString,$xmlrpcString,$xmlrpcString));
$useredit_sig = array(array($xmlrpcStruct,$xmlrpcString,$xmlrpcInt,$xmlrpcString,$xmlrpcString));
$userdel_sig = array(array($xmlrpcStruct,$xmlrpcString));

$groupmap = array(
	"group.list" => array(
		"function" => "NasAccessGroupList",
		"signature" => $grouplist_sig
	),
	"group.add" => array(
		"function" => "NasAccessGroupAdd",
		"signature" => $groupadd_sig
	),
	"group.edit" => array(
		"function" => "NasAccessGroupEdit",
		"signature" => $groupedit_sig
	),
	"group.del" => array(
		"function" => "NasAccessGroupDel",
		"signature" => $groupdel_sig
	)
);

$usermap = array(
	"user.list" => array(
		"function" => "NasAccessUserList",
		"signature" => $userlist_sig
	),
	"user.add" => array(
		"function" => "NasAccessUserAdd",
		"signature" => $useradd_sig
	),
	"user.edit" => array(
		"function" => "NasAccessUserEdit",
		"signature" => $useredit_sig
	),
	"user.del" => array(
		"function" => "NasAccessUserDel",
		"signature" => $userdel_sig
	)
);

/*volume signature*/
$volumelist_sig = array(array($xmlrpcStruct));
$volumememberlist_sig = array(array($xmlrpcStruct));
$volumeadd_sig = array(array($xmlrpcStruct,$xmlrpcString,$xmlrpcString,$xmlrpcString,$xmlrpcString,$xmlrpcString,$xmlrpcInt,$xmlrpcString));
$volumeedit_sig = array(array($xmlrpcStruct,$xmlrpcString,$xmlrpcString,$xmlrpcString,$xmlrpcInt));
$volumedel_sig = array(array($xmlrpcStruct,$xmlrpcString));

$volumemap = array(
	"volume.list" => array(
		"function" => "NasVolumeList",
		"signature" => $volumelist_sig
	),
	"volumemember.list" => array(
		"function" => "NasVolumeMemberList",
		"signature" => $volumememberlist_sig
	),
	"volume.add" => array(
		"function" => "NasVolumeAdd",
		"signature" => $volumeadd_sig
	),
	"volume.edit" => array(
		"function" => "NasVolumeEdit",
		"signature" => $volumeedit_sig
	),
	"volume.del" => array(
		"function" => "NasVolumeDel",
		"signature" => $volumedel_sig
	)
);

/*share signature*/
$sharememberlist_sig = array(array($xmlrpcStruct));

$winsharelist_sig = array(array($xmlrpcStruct));
$winshareadd_sig = array(array($xmlrpcStruct,$xmlrpcString,$xmlrpcString,$xmlrpcString));
$winshareedit_sig = array(array($xmlrpcStruct,$xmlrpcString,$xmlrpcString,$xmlrpcString,$xmlrpcString));
$winsharedel_sig = array(array($xmlrpcStruct,$xmlrpcString));

$unixsharelist_sig = array(array($xmlrpcStruct));
$unixshareadd_sig = array(array($xmlrpcStruct,$xmlrpcString,$xmlrpcString,$xmlrpcString,$xmlrpcString,$xmlrpcString));
$unixshareedit_sig = array(array($xmlrpcStruct,$xmlrpcString,$xmlrpcString,$xmlrpcString,$xmlrpcString,$xmlrpcString,$xmlrpcString));
$unixsharedel_sig = array(array($xmlrpcStruct,$xmlrpcString));

$winsharemap = array(
	"sharemember.list" => array(
		"function" => "NasShareMemberList",
		"signature" => $sharememberlist_sig
	),
	"winshare.list" => array(
		"function" => "NasWinShareList",
		"signature" => $winsharelist_sig
	),
	"winshare.add" => array(
		"function" => "NasWinShareAdd",
		"signature" => $winshareadd_sig
	),
	"winshare.edit" => array(
		"function" => "NasWinShareEdit",
		"signature" => $winshareedit_sig
	),
	"winshare.del" => array(
		"function" => "NasWinShareDel",
		"signature" => $winsharedel_sig
	)
);

$unixsharemap = array(
	"unixshare.list" => array(
		"function" => "NasUnixShareList",
		"signature" => $unixsharelist_sig
	),
	"unixshare.add" => array(
		"function" => "NasUnixShareAdd",
		"signature" => $unixshareadd_sig
	),
	"unixshare.edit" => array(
		"function" => "NasUnixShareEdit",
		"signature" => $unixshareedit_sig
	),
	"unixshare.del" => array(
		"function" => "NasUnixShareDel",
		"signature" => $unixsharedel_sig
	)
);

/*service signature*/
$smblist_sig = array(array($xmlrpcStruct));
$smbedit_sig = array(array($xmlrpcStruct,$xmlrpcString,$xmlrpcString));

$nfslist_sig = array(array($xmlrpcStruct));
$nfsedit_sig = array(array($xmlrpcStruct,$xmlrpcString,$xmlrpcInt));

$ftplist_sig = array(array($xmlrpcStruct));
$ftpedit_sig = array(array($xmlrpcStruct,$xmlrpcString,$xmlrpcInt,$xmlrpcInt,$xmlrpcString,$xmlrpcInt,$xmlrpcInt));

$iscsilist_sig = array(array($xmlrpcStruct));
$iscsiset_sig = array(array($xmlrpcStruct,$xmlrpcString));

$iscsidevicememberlist_sig = array(array($xmlrpcStruct));
$iscsifilememberlist_sig = array(array($xmlrpcStruct));

$iscsiadd_sig = array(array($xmlrpcStruct,$xmlrpcString,$xmlrpcString,$xmlrpcInt,$xmlrpcString));
$iscsiedit_sig = array(array($xmlrpcStruct,$xmlrpcString,$xmlrpcString,$xmlrpcInt,$xmlrpcString));
$iscsidel_sig = array(array($xmlrpcStruct,$xmlrpcString));

$servicemap = array(
	"smb.list" => array(
		"function" => "NasSmbList",
		"signature" => $smblist_sig
	),
	"smb.edit" => array(
		"function" => "NasSmbEdit",
		"signature" => $smbedit_sig
	),
	"nfs.list" => array(
		"function" => "NasNfsList",
		"signature" => $nfslist_sig
	),
	"nfs.edit" => array(
		"function" => "NasNfsEdit",
		"signature" => $nfsedit_sig
	),
	"ftp.list" => array(
		"function" => "NasFtpList",
		"signature" => $ftplist_sig
	),
	"ftp.edit" => array(
		"function" => "NasFtpEdit",
		"signature" => $ftpedit_sig
	),
	"iscsi.list" => array(
		"function" => "NasIscsiList",
		"signature" => $iscsilist_sig
	),
	"iscsi.set" => array(
		"function" => "NasIscsiSet",
		"signature" => $iscsiset_sig
	),
	"iscsidevicemember.list" => array(
		"function" => "NasIscsiDeviceMemberList",
		"signature" => $iscsidevicememberlist_sig
	),
	"iscsifilemember.list" => array(
		"function" => "NasIscsiFileMemberList",
		"signature" => $iscsifilememberlist_sig
	),
	"iscsi.add" => array(
		"function" => "NasIscsiAdd",
		"signature" => $iscsiadd_sig
	),	
	"iscsi.edit" => array(
		"function" => "NasIscsiEdit",
		"signature" => $iscsiedit_sig
	),
	"iscsi.del" => array(
		"function" => "NasIscsiDel",
		"signature" => $iscsidel_sig
	)
);


/*system manage*/

$statuslist_sig = array(array($xmlrpcStruct));
$servicelist_sig = array(array($xmlrpcStruct));
$bootset_sig = array(array($xmlrpcStruct,$xmlrpcString));
$nastest_sig = array(array($xmlrpcStruct));
$networklist_sig = array(array($xmlrpcStruct));
$networkset_sig = array(array($xmlrpcStruct,$xmlrpcString,$xmlrpcString,$xmlrpcString,$xmlrpcString,$xmlrpcString));
$testfunction_sig = array(array($xmlrpcStruct,$xmlrpcString));

$systemmap = array(
	"nas.test" => array(
		"function" => "NasTest",
		"signature" => $nastest_sig
	),
	"status.list" => array(
		"function" => "NasStatusList",
		"signature" => $statuslist_sig
	),
	"service.list" => array(
		"function" => "NasServiceList",
		"signature" => $servicelist_sig
	),
	"boot.set" => array(
		"function" => "NasBootSet",
		"signature" => $bootset_sig
	),
	"network.list" => array(
		"function" => "NasNetworkList",
		"signature" => $networklist_sig
	),
	"network.set" => array(
		"function" => "NasNetworkSet",
		"signature" => $networkset_sig
	),
	"test.function" => array(
		"function" => "TestFunction",
		"signature" => $testfunction_sig
	),	
);
	


/*xmlrpc server dispatch map*/
$xml_dispatch_map = array_merge(
	(array)$groupmap,
	(array)$usermap,
	(array)$volumemap,
	(array)$winsharemap,
	(array)$unixsharemap,
	(array)$servicemap,
	(array)$systemmap
);

// When an XML-RPC request is sent to this script, it can be found in the
// raw post data.
/*
$request_xml = $HTTP_RAW_POST_DATA;
if (empty($request_xml))
{
	//echo "empty msg\n";
	die;
}

*/
// Create XMLRPC server.
$xmlrpc_server = new xmlrpc_server($xml_dispatch_map, false);

// Process request.
$xmlrpc_server->service($request_xml);
?>
