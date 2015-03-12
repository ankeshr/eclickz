<?php

		/*
				________________________________________________________________________________

						  PROGRAM : ASP : Admin : XML Feeds Admin Center (PHP)
        VERSION : 2.11
        Purpose :
      ________________________________________________________________________________
         Copyright and Legal Notices:

           All source code, images, programs, files included in this distribution
           Copyright (c) eClickZ Interactive Services LLC.  All Rights Reserved.

           Use, distibution, sale, or access to this program is forbidden without
           permission, except were permitted by the license agreement.
      ________________________________________________________________________________
         Special Instructions and notes...
      ________________________________________________________________________________
   */

		global $PROGRAMS;
		$PROGRAMS[ 'xmlfeeds_admin' ] = 1;
		function config_xmlfeeds_admin ()
		{
				global $config;
				global $language;
				global $vars;
				global $menuitems;
				global $SCRIPT_NAME;
				global $ADMIN_TYPE;
				if ( $ADMIN_TYPE != "administrator" ) {
						return;
				}
				$vars[ 'xmlfeeds_admin' ][ '50' ]                        = array ( "Backfill Results Options" , "HEADER" , "" );
				$vars[ 'xmlfeeds_admin' ][ '52:supplemental_results' ]   = array ( "Do you wish to use <B>Backup Results</B> for your own database?" , "SELECT" , "NO|Live XML|Live XML via CURL (PHP5/Curl Required)|MetaServer|Dumped Feed" );
				$vars[ 'xmlfeeds_admin' ][ '53:xml_parser' ]             = array ( "Which <b>Live XML</b> paring method should be used" , "SELECT" , "PHP Native (Requires XPAT & XML Support)|Internal XML Parser" );
				$vars[ 'xmlfeeds_admin' ][ '60:metaserver_timeout' ]     = array ( "Retrieval Timeout (seconds)" , "TEXT" , "2" , "" );
				$vars[ 'xmlfeeds_admin' ][ '90' ]                        = array ( "<b>MetaServer Settings</b><br /><i>If you have a dedicated server, with shell access and have perl with threading support installed, you may elect to use the iWeb MetaServer daemon for remote XML Feed grabbing</i>" , "HEADER" , "" );
				$vars[ 'xmlfeeds_admin' ][ '91:metaserver_ip' ]          = array ( "MetaServer IP Address" , "TEXT" , "127.0.0.1" , "" );
				$vars[ 'xmlfeeds_admin' ][ '91:metaserver_port' ]        = array ( "MetaServer Port" , "TEXT" , "7070" , "" );
				$vars[ 'xmlfeeds_admin' ][ '92:metaserver_num_servers' ] = array ( "Number of MetaServers to run" , "TEXT" , "10" , "" );
				$vars[ 'xmlfeeds_admin' ][ '93:metaserver_num_clients' ] = array ( "Max # MetaServer Children (each server)" , "TEXT" , "20" , "" );
				$vars[ 'xmlfeeds_admin' ][ '95:max_related_keywords' ]   = array ( "Max # Related Keywords " , "TEXT" , "10" , "" );
				## Dynamic Menu Options
				$menuitems[ 'xml_feeds' ][ 'title' ]                                                               = $language[ 'xmlfeeds_admin' ];
				$menuitems[ 'xml_feeds' ][ 'categories' ][ $language[ 'tab_xml_admin' ] ]                          = 1;
				$menuitems[ 'xml_feeds' ][ 'items' ][ $language[ 'tab_xml_admin' ] ][ $language[ 'add_feed' ] ]    = "$SCRIPT_NAME?admin=xml&action=add";
				$menuitems[ 'xml_feeds' ][ 'items' ][ $language[ 'tab_xml_admin' ] ][ $language[ 'list_feeds' ] ]  = "$SCRIPT_NAME?admin=xml&action=list_feeds";
				$menuitems[ 'xml_feeds' ][ 'items' ][ $language[ 'tab_xml_admin' ] ][ $language[ 'signup_link' ] ] = "$SCRIPT_NAME?admin=xml&action=signup";
				$menuitems[ 'xml_feeds' ][ 'items' ][ $language[ 'tab_xml_admin' ] ][ $language[ 'usage_help' ] ]  = "$SCRIPT_NAME?admin=xml&action=help";
		}

?>