<?php

		/*
		________________________________________________________________________________

						PROGRAM : ASP : Admin : Account Options Admin Center (PHP)
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
		$PROGRAMS[ 'acct_admin' ] = 1;
		function config_acct_admin ()
		{
				global $oSQL;
				global $config;
				global $language;
				global $vars;
				global $menuitems;
				global $SCRIPT_NAME;
				global $ADMIN_TYPE;
				$SQL                                             = "SELECT template_name FROM " . TEMPLATES_TABLE . " WHERE program='acct_admin' AND theme='default' AND template_type = 'EMAIL'";
				$titles                                          = $oSQL->search ( array ( "SQL" => $SQL , "Return" => 'array' ) );
				$opts                                            = "|" . join ( "|" , $titles );
				$SQL                                             = "SELECT count(*) FROM " . ACCOUNTS_TABLE;
				$total_users                                     = $oSQL->search ( array ( "SQL" => $SQL , "Return" => "single" ) );
				$SQL                                             = "SELECT COUNT(*) FROM " . DEPOSITS_TABLE;
				$DEPOSITS                                        = $oSQL->search ( array ( "SQL" => $SQL , "Return" => "single" ) );
				$SQL                                             = "SELECT COUNT(*) FROM " . WITHDRAWLS_TABLE;
				$WITHDRAWLS                                      = $oSQL->search ( array ( "SQL" => $SQL , "Return" => "single" ) );
				$SQL                                             = "SELECT group_name FROM " . USER_GROUPS_TABLE . " ORDER BY group_name";
				$grouplist                                       = $oSQL->search ( array ( "SQL" => $SQL , "Return" => "array" ) );
				$SQL                                             = "SELECT trans_type, sum(trans_amount) AS total FROM " . TRANSACTIONS_TABLE . " GROUP BY trans_type";
				$deplist                                         = $oSQL->search ( array ( "SQL" => $SQL ) );
				$vars[ 'eMails' ][ '01' ]                        = array ( "eMail Options" , "HEADER" , "" );
				$vars[ 'eMails' ][ '10:jh_welcome_email' ]       = array ( "Email to send to new member when accounts are created" , "SELECT" , $opts );
				$vars[ 'eMails' ][ '11:jh_welcome_email_admin' ] = array ( "Email to send to administrator hen accounts are created" , "SELECT" , $opts );
				$vars[ 'eMails' ][ '12:jh_admin_payment_email' ] = array ( "Email to send to administrator after a deposit" , "SELECT" , $opts );
				$vars[ 'eMails' ][ '13:jh_user_payment_email' ]  = array ( "Email to send to user after deposit" , "SELECT" , $opts );
				$vars[ 'eMails' ][ '14:jh_autobill_email' ]      = array ( "Email to send to a user after they have been <b>autobilled</b>" , "SELECT" , $opts );
				$vars[ 'eMails' ][ '15:jh_withdrawl_email' ]     = array ( "Email to send to administrator to alert on a withdrawl" , "SELECT" , $opts );
				$vars[ 'eMails' ][ '30' ]                        = array ( "Support Request Settings (Jackhammer Users Only)" , "HEADER" , "" );
				$vars[ 'eMails' ][ '31:jh_new_ticket_email' ]    = array ( "Email to send to administrator on new Support Ticket Creations" , "SELECT" , $opts );
				$vars[ 'eMails' ][ '32:jh_ticket_reply_email' ]  = array ( "Email to send on all support ticket replies" , "SELECT" , $opts );
				$vars[ 'templates' ][ 'LAYOUT' ]                 = "INBOX LIST|INBOX MENU|INBOX MESSAGE|VIEW PROFILE";
				$vars[ 'templates' ][ 'EMAIL' ]                  = "Welcome";
				## Dynamic Menu ##
				if ( $ADMIN_TYPE == "designer" ) {
						return;
				}
				// "administrator" only options
				if ( $ADMIN_TYPE == "administrator" ) {
						$menuitems[ 'accounts' ][ 'title' ]                                                                 = $language[ 'acct_admin' ];
						$menuitems[ 'accounts' ][ 'categories' ][ $language[ 'tab_users' ] ]                                = 1;
						$menuitems[ 'accounts' ][ 'items' ][ $language[ 'tab_users' ] ][ $language[ 'add_acct_prompt' ] ]   = "$SCRIPT_NAME?admin=accounts&action=edit_account";
						$menuitems[ 'accounts' ][ 'items' ][ $language[ 'tab_users' ] ][ $language[ 'search_users_link' ] ] = "$SCRIPT_NAME?admin=accounts&action=search";
						$menuitems[ 'accounts' ][ 'categories' ][ $language[ 'tab_groups' ] ]                               = 3;
						$menuitems[ 'accounts' ][ 'items' ][ $language[ 'tab_groups' ] ][ $language[ 'new_group_link' ] ]   = "$SCRIPT_NAME?admin=accounts&action=edit_group";
						foreach ( $grouplist as $group ) {
								$link_to                                                                     = urlencode ( $group );
								$varname                                                                     = $group;
								$varname                                                                     = cleanup_string ( $varname );
								$varname                                                                     = preg_replace ( "/\-/" , "_" , $varname );
								$menuitems[ 'accounts' ][ 'items' ][ $language[ 'tab_groups' ] ][ $varname ] = "$SCRIPT_NAME?admin=accounts&group=$link_to&action=edit_group";
						}
						$menuitems[ 'accounts' ][ 'categories' ][ $language[ 'tab_finances' ] ]                                                    = 4;
						$menuitems[ 'accounts' ][ 'items' ][ $language[ 'tab_finances' ] ][ "$language[queue_pending_withdrawls]  ($WITHDRAWLS)" ] = "$SCRIPT_NAME?admin=accounts&action=withdrawls";
						$menuitems[ 'accounts' ][ 'items' ][ $language[ 'tab_finances' ] ][ "$language[queue_pending_deposits] ($DEPOSITS)" ]      = "$SCRIPT_NAME?admin=accounts&action=deposits";
						$menuitems[ 'accounts' ][ 'items' ][ $language[ 'tab_finances' ] ][ $language[ 'deposit_summary_header' ] ]                = "$SCRIPT_NAME?admin=accounts&action=deposit_summary";
						$menuitems[ 'accounts' ][ 'categories' ][ $language[ 'deposits_by_type' ] ]                                                = 5;
						foreach ( $deplist as $type ) {
								if ( !$type[ 'trans_type' ] ) {
										$type[ 'trans_type' ] = "Unspecified";
								}
								$link_to                                                                                                                            = urlencode ( $type[ 'trans_type' ] );
								$varname                                                                                                                            = $type[ 'trans_type' ];
								$varname                                                                                                                            = cleanup_string ( $varname );
								$varname                                                                                                                            = preg_replace ( "/\-/" , "_" , $varname );
								$menuitems[ 'accounts' ][ 'items' ][ $language[ 'deposits_by_type' ] ][ "$type[trans_type] ($language[dollar_sign] $type[total])" ] = "$SCRIPT_NAME?admin=accounts&action=deposit_summary&type=$link_to";
						}
				} // End "administrator" only options
				// Aff Manager Only Options //
				if ( $ADMIN_TYPE == "affiliate" ) {
						$menuitems[ 'accounts' ][ 'items' ][ $language[ 'tab_finances' ] ][ "$language[queue_pending_withdrawls]  ($WITHDRAWLS)" ] = "$SCRIPT_NAME?admin=accounts&action=withdrawls";
				}
		}

?>