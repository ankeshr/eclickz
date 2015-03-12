<?php
		@session_start ();
		/*
	________________________________________________________________________________

			     PROGRAM : ASP : Admin : Initializer (PHP)
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


		function init_system ()
		{
				################################################################################
				# Application Globals                                                          #
				################################################################################
				global $data_dir;
				global $config;
				global $language;
				global $templates;
				global $schemas;
				global $iSQL; // OLTP DB Connection (For writes and quick queries)
				global $oSQL; // OLAP DB Connection (For reports & long queries)
				global $aSQL; // OLAP DB Connection (For admin reports & long queries)
				global $SCRIPT_NAME;
				global $CGI;
				global $settings;
				global $ENV;
				global $COOKIES;
				global $UPLOADS;
				global $debug;
				global $remote;
				global $queries_run;
				$queries_run = array ();
				global $BENCHMARK;
				$BENCHMARK = array ();
				global $settings_found;
				global $mco;
				global $OUTPUT_FORMAT;
				global $ADMIN;
				/* Global variable created by Rajesh Ranjan  on 12/27/2010 for
			store the each partners result value */
				global $partners;
				$partners = array ();
				// Fatal Error Storage //
				$GLOBALS[ 'fatal_system_errors' ] = array ();
				################################################################################
				# Turn Debugger on/off                                                         #
				################################################################################
				$debug = 1;
				################################################################################
				#  Safely, Get any cookie, form, or environment values                         #
				################################################################################
				init_environment ();
				################################################################################
				#  Get Table Definitions                                                       #
				################################################################################
				define_tables ();
				################################################################################
				#  Skip connect and everything else if we're in install mode                   #
				################################################################################
				if ( !empty( $CGI[ 'admin' ] ) && $CGI[ 'admin' ] == "install" ) {
						return;
				}
				################################################################################
				#  Handle user language preferences, via cookie                                #
				################################################################################
				if ( !empty( $CGI[ 'lang' ] ) ) {
						setcookie ( "language" , $CGI[ 'lang' ] , 0 , "/" );
						$COOKIES[ 'language' ] = $CGI[ 'lang' ];
				}
				################################################################################
				# Read in the Config Options                                                   #
				################################################################################
				start_benchmark ( "Setup: Get Settings" );
				$theme         = '';
				$main_cfg_file = "$data_dir/config.inc";
				if ( preg_match ( "/admin/i" , $GLOBALS[ 'SCRIPT_NAME' ] ) ) {
						if ( !$CGI[ 'theme' ] ) {
								$theme = "default";
						}
				} else {
						if ( !empty( $CGI[ 'theme' ] ) ) {
								$theme = $CGI[ 'theme' ];
						} elseif ( !empty( $COOKIES[ 'theme' ] ) ) {
								$CGI[ 'theme' ] = $COOKIES[ 'theme' ];
								$theme          = $CGI[ 'theme' ];
						}
						if ( $theme && file_exists ( "$data_dir/config/$theme.inc" ) ) {
								$cfg_file = "$data_dir/config/$theme.inc";
								setcookie ( "theme" , $CGI[ 'theme' ] , 0 , "/" );
						} else {
								$CGI[ 'theme' ] = $theme = "default";
						}
				}
				// Always read in the main config file first, it has the system level settings
				if ( file_exists ( $main_cfg_file ) ) {
						include ( $main_cfg_file );
				} else {
						$created = fix_config ();
						if ( $created ) {
								include ( $main_cfg_file );
						} else {
								trigger_error ( "Configuration file $main_cfg_file does not exist.  Exiting" , E_USER_ERROR );
								exit;
						}
				}
				// Now, if the theme is not default, read in the theme's config file to override the visual settings //
				if ( $cfg_file != $main_cfg_file ) {
						$defaultsettings = $config;
						include ( $cfg_file );
						$themesettings = $config;
						$config        = array_merge ( $defaultsettings , $themesettings );
				}
				stop_benchmark ( "Setup: Get Settings" );
				################################################################################
				# Pre-Load Fraud Checks
				################################################################################
				if ( !$ADMIN && preload_fraud () ) {
						die( "Access Denied" );
				}
				################################################################################
				# Initialize the Cache-ing Engine                                              #
				################################################################################
				if ( $config[ 'php_cache' ] == "memcached" ) {
						$mco = new Memcache;
						if ( !isset( $config[ 'memc_server' ] ) ) {
								// memcached server list not set, connecting to default
								$mco->connect ( 'localhost' , 11211 ) or die ( "Could not connect" );
						} else {
								if ( isset( $config[ 'memc_port' ] ) ) $memcport = $config[ 'memc_port' ]; else $memcport = 11211;
								if ( !is_array ( $config[ 'memc_server' ] ) ) {
										$mco->connect ( $config[ 'memc_server' ] , $memcport ) or die ( "Could not connect" );;
								} else {
										shuffle ( $config[ 'memc_server' ] );
										while ( count ( $config[ 'memc_server' ] ) ) {
												$hhostname = array_shift ( $config[ 'memc_server' ] );
												$mco->addServer ( $hhostname , $memcport );
										}
								}
						}
				}
				################################################################################
				#  Database Module & Connection                                                #
				################################################################################
				if ( $config[ 'sql_driver' ] ) {
						//$replicavar = 1;//added By Rajesh on 27-jan-2011
						$driver = $config[ 'sql_driver' ];
						include_once ( $remote . "dsX/iSQL/drivers/$driver.inc" );
						start_benchmark ( "Setup: SQL Connect (1)" );
						$iSQL = new iSQL ( $config[ "dbhost" ] , $config[ "dbname" ] , $config[ "dbusername" ] , $config[ "dbpassword" ] , $config[ 'dbwritehost' ] );
						stop_benchmark ( "Setup: SQL Connect (1)" );
						/* Script is Modified By Raajesh Ranjan on 31-Jan-2011 for call the admin report.
									The following instance is separate for admin report only.
						*/
						start_benchmark ( "Setup: SQL Connect (1)" );
						$aSQL = new iSQL ( $config[ "admin_dbhost" ] , $config[ "admin_dbname" ] , $config[ "admin_dbusername" ] , $config[ "admin_dbpassword" ] , $config[ 'dbwritehost' ] );
						stop_benchmark ( "Setup: SQL Connect (1)" );
						/*End*/
						// Code added by Rajesh Ranjan on 29-Jan-2011 to check the last slave connection value
						$SQLCON     = "SELECT replica_no FROM  iweb_connection_log WHERE 1";
						$replicavar = $iSQL->search ( array ( "SQL" => $SQLCON , "Return" => "single" ) );
						/// connect to OLAP if defined.  Otherwise, olap and oltp are the same connection
						if ( ( $config[ "olap_dbhost" ] && $config[ "olap_dbname" ] && $config[ "olap_dbusername" ] ) && ( ( $config[ "olap_dbhost" ] != $config[ "dbhost" ] ) || ( $config[ "olap_dbname" ] != $config[ "dbname" ] ) || ( $config[ "olap_dbusername" ] != $config[ "dbusername" ] ) )
						) {
								/*
															Code modified By Rajesh Ranjan on 27 jan 2011
															The following code change the database connection on each request, and distribute the load on different slaves database
									*/
								if ( $replicavar == 1 ) {
										// if slave-1 is in use change the connection to slave-2
										$logdate = date ( 'Y-m-d h:i:s' );
										$SQLEDIT = "UPDATE iweb_connection_log SET replica_no=2,logdate= '$logdate' WHERE c_id=1";
										$iSQL->run ( $SQLEDIT );
										$conection_obj = "Conection 1 olap_dbhost 1 URL-> " . $_SERVER[ "SERVER_ADDR" ];
										start_benchmark ( "Setup: SQL Connect (Slave-1)" );
										$oSQL = new iSQL ( $config[ "olap_dbhost" ] , $config[ "olap_dbname" ] , $config[ "olap_dbusername" ] , $config[ "olap_dbpassword" ] , $config[ 'dbwritehost' ] );
										stop_benchmark ( "Setup: SQL Connect (Slave-1)" );
										//write_conncetion_log($conection_obj);
								} else {
										// if slave-2 is in use change the connection to slave-1
										$logdate = date ( 'Y-m-d h:i:s' );
										$SQLEDIT = "UPDATE iweb_connection_log SET replica_no=1,logdate= '$logdate' WHERE c_id=1";
										$iSQL->run ( $SQLEDIT );
										$conection_obj = "Conection 2 olap_dbhost2 URL->" . $_SERVER[ "SERVER_ADDR" ];
										start_benchmark ( "Setup: SQL Connect (Slave-2)" );
										$oSQL = new iSQL ( $config[ "olap_dbhost2" ] , $config[ "olap_dbname2" ] , $config[ "olap_dbusername2" ] , $config[ "olap_dbpassword2" ] , $config[ 'dbwritehost' ] );
										stop_benchmark ( "Setup: SQL Connect (Slave-2)" );
										//write_conncetion_log($conection_obj);
								}
								if ( $_SERVER[ 'HTTP_HOST' ] == $config['admin_domain'] || $_SERVER[ 'REQUEST_URI' ] == '/admin/' ) {
										start_benchmark ( "Setup: SQL Connect (1)" );
										$aSQL = new iSQL ( $config[ "admin_dbhost" ] , $config[ "admin_dbname" ] , $config[ "admin_dbusername" ] , $config[ "admin_dbpassword" ] , $config[ 'dbwritehost' ] );
										stop_benchmark ( "Setup: SQL Connect (1)" );
										write_conncetion_log ( "Admin Report Setup: SQL Connect (4)" . $config[ "admin_dbhost" ] );
								}
						} else {
								$oSQL = new iSQL ( $config[ "dbhost" ] , $config[ "dbname" ] , $config[ "dbusername" ] , $config[ "dbpassword" ] , $config[ 'dbwritehost' ] );
								//$oSQL = $iSQL;
						}
				}
				################################################################################
				# Config / Lang / Templates: Pre-Read                                          #
				################################################################################
				// Read Saved Settings, if available //
				if ( ( $config[ 'php_cache' ] == "memcached" ) || ( $config[ 'php_cache' ] == "apc" ) ) {
						start_benchmark ( "Setup: Read MemCache" );
						// Remove saved settings, if requested //
						if ( !empty( $CGI[ 'reset_vars' ] ) ) {
								$mco->flush ();
								echo "Cache Reset";
								exit;
						}
						$tmpres = read_cache ( Array ( 'language' , 'templates' , 'schemas' ) );
						if ( isset( $tmpres[ 'language' ] ) && isset( $tmpres[ 'templates' ] ) && isset( $tmpres[ 'schemas' ] ) ) {
								$settings_found = 1;
								$language       = $tmpres[ 'language' ];
								$templates      = $tmpres[ 'templates' ];
								$schemas        = $tmpres[ 'schemas' ];
						}
						stop_benchmark ( "Setup: Read MemCache" );
				} elseif ( $config[ 'php_cache' ] == "Standard" ) {
						start_benchmark ( "Setup: Read Standard Cache" );
						if ( file_exists ( "$data_dir/phpsettings.inc" ) ) {
								include_once ( "$data_dir/phpsettings.inc" );
								$settings_found = 1;
						}
						stop_benchmark ( "Setup: Read Standard Cache" );
				}
				################################################################################
				#  Read in the language & Templates ... Save Settings to file                  #
				################################################################################
				if ( !$settings_found && $OUTPUT_FORMAT != "xml" ) {
						if ( $oSQL ) {
								// Allow the user to select their language
								if ( !empty( $COOKIES[ 'language' ] ) ) {
										$config[ 'default_language' ] = $COOKIES[ 'language' ];
								}
								$lname = $config[ 'default_language' ];
								start_benchmark ( "Setup: Read Language" );
								$langSQL  = "SELECT setting_name, current_value FROM iweb_language WHERE language = '$lname'";
								$language = $oSQL->search ( array ( "SQL" => $langSQL , "Return" => 'fullhash' , "override_error" => 1 ) );
								if ( $oSQL->error ) {
										array_push ( $GLOBALS[ 'fatal_system_errors' ] , $oSQL->error );
								}
								stop_benchmark ( "Setup: Read Language" );
								start_benchmark ( "Setup: Read Templates" );
								$templateSQL = "SELECT * FROM iweb_internal_templates ORDER BY template_name ASC";
								$tdata       = $oSQL->search ( array ( "SQL" => $templateSQL , "override_error" => 1 ) );
								if ( $oSQL->error ) {
										array_push ( $GLOBALS[ 'fatal_system_errors' ] , $oSQL->error );
								}
								$templates = array ();
								foreach ( $tdata as $template ) {
										$program                                           = $template[ 'program' ];
										$theme                                             = $template[ 'theme' ];
										$name                                              = $template[ 'template_name' ];
										$type                                              = $template[ 'template_type' ];
										$templates[ $program ][ $theme ][ $type ][ $name ] = $template[ 'template_text' ];
								}
								stop_benchmark ( "Setup: Read Templates" );
								start_benchmark ( "Setup: Read Schemas" );
								$defSQL  = "SELECT * FROM iweb_table_def ORDER BY table_name, schema_name, form_order";
								$defdata = $oSQL->search ( array ( "SQL" => $defSQL , "override_error" => 1 ) );
								if ( $oSQL->error ) {
										array_push ( $GLOBALS[ 'fatal_system_errors' ] , $oSQL->error );
								}
								$schemas = array ();
								foreach ( $defdata as $schema ) {
										$sname = $schema[ 'schema_name' ];
										if ( empty( $schemas[ $sname ] ) ) {
												$schemas[ $sname ] = array ();
										}
										array_push ( $schemas[ $sname ] , $schema );
								}
								stop_benchmark ( "Setup: Read Schemas" );
						}
						if ( ( $config[ 'php_cache' ] == "memcached" ) || ( $config[ 'php_cache' ] == "apc" ) ) {
								write_cache ( "language" , $language );
								write_cache ( "templates" , $templates );
								write_cache ( "schemas" , $schemas );
						} elseif ( $config[ 'php_cache' ] == "Standard" ) {
								// Dump $language, $templates out to a temp file //
								$l_dump  = var_export ( $language , TRUE );
								$t_dump  = var_export ( $templates , TRUE );
								$s_dump  = var_export ( $schemas , TRUE );
								$tmpfile = fopen ( "$data_dir/phpsettings.inc" , "w" );
								$fp      = fwrite ( $tmpfile , '<?php' );
								$fp      = fwrite ( $tmpfile , "\n\n" );
								$fp      = fwrite ( $tmpfile , '$language = ' . $l_dump . ';' );
								$fp      = fwrite ( $tmpfile , "\n\n" );
								$fp      = fwrite ( $tmpfile , '$templates = ' . $t_dump . ';' );
								$fp      = fwrite ( $tmpfile , "\n\n" );
								$fp      = fwrite ( $tmpfile , '$schemas = ' . $s_dump . ';' );
								$fp      = fwrite ( $tmpfile , "\n\n" );
								$fp      = fwrite ( $tmpfile , '?>' );
								fclose ( $tmpfile );
								if ( isset( $config[ 'config_sync_script' ] ) && !empty( $config[ 'config_sync_script' ] ) && strcasecmp ( $config[ 'config_sync_script' ] , "none" ) ) {
										exec ( "$data_dir/../../" . $config[ 'config_sync_script' ] );
								}
						}
				}
				################################################################################
				#  Get a seed for later, in case we need it.                                   #
				################################################################################
				mt_srand ( time () );
				dci ();
		}

		/*
		Created By Rajesh on 27-Jan-2010 for create the connection log.
		*/

		function write_conncetion_log ( $contents )
		{
				$logFile = "/var/www/html/connectionlog.txt";
				$fh = fopen ( $logFile , 'a+' ) or die( "can't open file" );
				$stringData = "[" . date ( 'Y-m-d H:i:s' ) . "], Connection Type : " . $contents . chr ( 13 ) . chr ( 13 ) . chr ( 13 ) . chr ( 13 );
				fwrite ( $fh , $stringData );
				fclose ( $fh );
		}

?>
