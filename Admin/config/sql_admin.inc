<?php

		/*
				________________________________________________________________________________

						  PROGRAM : ASP : Admin : mySQL Database Admin Center (PHP)
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
		$PROGRAMS[ 'sql_admin' ] = 1;
		function config_sql_admin ()
		{
				global $oSQL;
				global $config;
				global $language;
				global $vars;
				global $menuitems;
				global $SCRIPT_NAME;
				global $ADMIN_TYPE;
				$SQL = "SELECT VERSION()";
				$v   = $oSQL->search ( array ( "SQL" => $SQL , "Return" => "single" ) );
				list ( $v1 , $junk ) = split ( '-' , $v );
				$varr    = preg_split ( "/\./" , $v1 , 3 );
				$version = $varr[ 0 ] . "." . $varr[ 1 ];
				if ( $ADMIN_TYPE != "administrator" ) {
						return;
				}
				$vars[ 'Database' ][ '00' ]                                             = array ( "General Database Settings" , "HEADER" , "" );
				$vars[ 'Database' ][ '01:sql_driver' ]                                  = array ( "Which Database Driver for SQL Databases" , "SELECT" , "mysql" );
				$vars[ 'Database' ][ '02:mysql_version' ]                               = array ( "mySQL Database Version" , "TEXT" , $version );
				$vars[ 'Database' ][ '10' ]                                             = array ( "Transaction (OLTP) Database Settings" , "HEADER" , "" );
				$vars[ 'Database' ][ '12:dbhost' ]                                      = array ( "Database Server Name (use 'localhost' if on the same machine)" , "TEXT" , "localhost" );
				$vars[ 'Database' ][ '13:dbname' ]                                      = array ( "Database Name" , "TEXT" , "" );
				$vars[ 'Database' ][ '14:dbusername' ]                                  = array ( "Database User ID" , "TEXT" , "" );
				$vars[ 'Database' ][ '15:dbpassword' ]                                  = array ( "Database Password" , "TEXT" , "" );
				$vars[ 'Database' ][ '20' ]                                             = array ( "Reporting (OLAP) Database Settings" , "HEADER" , "" );
				$vars[ 'Database' ][ '22:olap_dbhost' ]                                 = array ( "Database Server Name (use 'localhost' if on the same machine)" , "TEXT" , "localhost" );
				$vars[ 'Database' ][ '23:olap_dbname' ]                                 = array ( "Database Name" , "TEXT" , "" );
				$vars[ 'Database' ][ '24:olap_dbusername' ]                             = array ( "Database User ID" , "TEXT" , "" );
				$vars[ 'Database' ][ '25:olap_dbpassword' ]                             = array ( "Database Password" , "TEXT" , "" );
				$menuitems[ 'sql' ][ 'title' ]                                          = $language[ 'sql_admin' ];
				$menuitems[ 'sql' ][ 'categories' ][ $language[ 'list_schemas_link' ] ] = "$SCRIPT_NAME?admin=sql&action=list_schemas";
		}

?>