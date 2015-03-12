<?php

 		/*

	    	________________________________________________________________________________

			     PROGRAM : ASP : Admin : PPC Sales Manager Admin Center
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


		################################################################################
		# Validate the user
		################################################################################
		function ppcsales_validate ()
		{
				global $config;
				global $PHP_AUTH_USER;
				global $PHP_AUTH_PW;
				global $REMOTE_USER;
				global $ADMIN_TYPE;
				global $CGI;
				global $COOKIES;
				global $oSQL;
				// Possibly got a login form login ?
				if ( $CGI[ 'adminuser' ] && $CGI[ 'adminpassword' ] ) {
						$PHP_AUTH_USER = $CGI[ 'adminuser' ];
						$PHP_AUTH_PW   = $CGI[ 'adminpassword' ];
				} elseif ( $COOKIES[ 'ppcmanageruser' ] && $COOKIES[ 'ppcmanagerpassword' ] ) {
						$PHP_AUTH_USER = $COOKIES[ 'ppcmanageruser' ];
						$PHP_AUTH_PW   = $COOKIES[ 'ppcmanagerpassword' ];
				}
				// maybe we have caught authentication data in $_SERVER['REMOTE_USER']
				if ( preg_match ( '/Basic\s+(.*)$/i' , $REMOTE_USER , $matches ) ) {
						list( $uname , $pwd ) = explode ( ':' , base64_decode ( $matches[ 1 ] ) );
						$PHP_AUTH_USER = strip_tags ( $uname );
						$PHP_AUTH_PW   = strip_tags ( $pwd );
				}
				if ( !isset( $PHP_AUTH_USER ) ) {
						header ( 'WWW-Authenticate: Basic realm="Valid PPC Manager Username/Password Required"' );
						header ( 'HTTP/1.0 401 Unauthorized' );
						//Result if user hits cancel button
						login_error ();
				} else {
						$accts_table  = ACCOUNTS_TABLE;
						$groups_table = GROUP_ASSIGNMENTS_TABLE;
						$SQL          = "
               SELECT count(*) FROM $accts_table, $groups_table WHERE $accts_table.acct_id = '$PHP_AUTH_USER' AND $accts_table.acct_password = '$PHP_AUTH_PW'
               AND $groups_table.group_name = 'PPC Manager'
               AND $groups_table.acct_id = $accts_table.acct_id
            ";
						$valid        = $oSQL->search ( array ( "SQL" => $SQL , "Return" => "single" ) );
						if ( $valid ) {
								return 1;
						} else {
								//This re-asks three times the login and password.
								header ( 'WWW-Authenticate: Basic realm="Valid PPC Manager Username/Password Required"' );
								header ( 'HTTP/1.0 401 Unauthorized' );
								//Result if user does not give good login and pass
								login_error ();
						}
				}
		}


		class ppcmanagerAdmin
		{

				var $output;

				function ppcmanagerAdmin ()
				{
						// Includes & Setup
						include_once ( "dsX/iSQL/Record/Listing.inc" );
						include_once ( "dsX/iSQL/Record/Keyword.inc" );
						include_once ( "dsX/iSQL/Record/CategoryNavigator.inc" );
						include_once ( "dsX/Admin/category-functions.inc" );
						include_once ( "dsX/Admin/hyperseek-crons.inc" );
						include_once ( "dsX/Admin/jackhammer.inc" );
						include_once ( "dsX/Admin/reports.inc" );
						include_once ( "dsX/Admin/affiliate-reports.inc" );
				}


				################################################################################
				# Run the requested action
				################################################################################
				function go ()
				{
						global $CGI;
						$action = $CGI[ 'action' ];
						if ( !$action ) {
								$action = "mainmenu";
						}
						if ( method_exists ( $this , $action ) ) {
								call_user_func ( array ( &$this , $action ) );
						} elseif ( function_exists ( $action ) ) {
								call_user_func ( $action );
						} else {
								admin_output ( "ERROR" , "Invalid Action: $action" );
								exit;
						}
				}



				// --------------------------------------------------------------------- //
				//  Page, with frames
				// --------------------------------------------------------------------- //
				function mainmenu ()
				{
						global $SCRIPT_NAME;
						echo "

               <html>
               <head>
                   <title>Account Manager</title>
               </head>

               <frameset rows='125,*'>
                   <frame name='work' frameborder='0' margin='0' scrolling='no' src='$SCRIPT_NAME?admin=ppcmanager&action=topmenu'>
                   <frame name='main' frameborder='0' margin='0' scrolling='auto' src='$SCRIPT_NAME?admin=ppcmanager&action=adminhome'>
               </frameset>

               </html>

            ";
						exit;
				}


				// --------------------------------------------------------------------- //
				//  Bottom Frame
				// --------------------------------------------------------------------- //
				function adminhome ()
				{
						echo "

               <html>
               <head>
                   <title>Account Manager Home Page</title>

                   <style>

                       body,td {
                          font-family:verdana,arial,helvetica;
                          font-size:9pt;
                          color:black;
                       }

                       a { color: navy; }
                       a.visited { color:navy; }

                   </style>

               </head>
               <body style='background:#f5f5f5; padding:10px;'>
                   Home Page Links
               </body>
               </html>
            ";
						exit;
				}



				// --------------------------------------------------------------------- //
				//  Top frame (MENU)
				// --------------------------------------------------------------------- //
				function topmenu ()
				{
						global $oSQL;
						global $SCRIPT_NAME;
						global $PHP_AUTH_USER;
						global $my_accounts;
						// Get the accounts //
						$account_array   = array ();
						$account_options = '';
						$accounts        = '';
						if ( !empty( $my_accounts ) ) {
								foreach ( $my_accounts as $id => $name ) {
										array_push ( $account_array , $id );
										$account_options .= "
                          <option value='$id'>$id ($name)</option>
                        ";
								}
								$accounts = "
                      <td>
                       <form  target='main' action='$SCRIPT_NAME' style='margin:0px;'>
                             <select name='acct_id'>
                                $account_options
                             </select>
                             <input type='hidden' name='admin' value='ppcmanager' />
                             <input type='hidden' name='action' value='view_account' />
                             <input type='submit' value='Manage' />
                       </form>
                      </td>
                    ";
						}
						// Get pending links //
						$pending_links = '';
						if ( !empty( $account_array ) ) {
								$list              = join ( "','" , array_keys ( $my_accounts ) );
								$SQL               = "SELECT count(*) FROM " . TEMP_TABLE . " WHERE account IN('$list') AND submission_type = 'NEW'";
								$num_pending_links = $oSQL->search ( array ( "SQL" => $SQL , "Return" => "single" ) );
								if ( $num_pending_links >= 1 ) {
										$pending_links = "
                          <td>|</td>
                          <td>
                           <a href='$SCRIPT_NAME?admin=hyperseek&action=submissions&type=NEW' target='main'>New Links ($num_pending_links)</a>
                          </td>
                        ";
								}
						}
						// Get change requests //
						$pending_changes = '';
						if ( !empty( $account_array ) ) {
								$list                = join ( "','" , array_keys ( $my_accounts ) );
								$SQL                 = "SELECT count(*) FROM " . TEMP_TABLE . " WHERE account IN('$list') AND submission_type = 'CHANGE'";
								$num_pending_changes = $oSQL->search ( array ( "SQL" => $SQL , "Return" => "single" ) );
								if ( $num_pending_changes >= 1 ) {
										$pending_changes = "
                          <td>|</td>
                          <td>
                           <a href='$SCRIPT_NAME?admin=hyperseek&action=submissions&type=CHANGE' target='main'>Change Requests ($num_pending_changes)</a>
                          </td>
                        ";
								}
						}
						// Get pending bids //
						$pending_bids = '';
						if ( !empty( $account_array ) ) {
								$list             = join ( "','" , array_keys ( $my_accounts ) );
								$SQL              = "SELECT count(*) FROM " . PENDING_BIDS_TABLE . " WHERE account IN('$list')";
								$num_pending_bids = $oSQL->search ( array ( "SQL" => $SQL , "Return" => "single" ) );
								if ( $num_pending_bids >= 1 ) {
										$pending_bids = "
                          <td>|</td>
                          <td>
                           <a href='$SCRIPT_NAME?admin=ppcmanager&action=pending_bids' target='main'>New Bids ($num_pending_bids)</a>
                          </td>
                        ";
								}
						}
						echo "

               <html>
               <head>
                   <title>Account Manager Main Menu</title>
                   <style>

                       body,td {
                          font-family:verdana,arial,helvetica;
                          font-size:9pt;
                          color:white;
                       }

                       a { color: #e0e0e0; }
                       a.visited { color:#e0e0e0; }

                   </style>

               </head>
               <body style='border:1px outset silver; margin:0px; background:#414141; padding:3px;'>
                   <fieldset>
                      <legend style='color:gold'>Account Manager Main Menu</legend>
                      <table border='0' cellpadding='5'>
                         <tr>
                           <td><b>Accounts:</b></td>
                           $accounts
                           $pending_links
                           $pending_changes
                           $pending_bids
                           <td>|</td>
                           <td><a href='$SCRIPT_NAME?admin=ppcmanager&action=massmail' target='main'>Mass eMail</a></td>
                         </tr>
                      </table>

                      <table border='0' cellpadding='5'>
                         <tr>
                           <td><b>Reports:</b></td>
                           <td><a href='$SCRIPT_NAME?admin=ppcmanager&action=deposit_report' target='main'>Deposits</a></td>
                           <td><a href='$SCRIPT_NAME?admin=ppcmanager&action=earnings_report' target='main'>Earnings</a></td>
                           <td><a href='$SCRIPT_NAME?admin=ppcmanager&action=click_report' target='main'>Clicks</a></td>
                         </tr>
                      </table>

                      <table border='0' cellpadding='5'>
                         <tr>
                           <td><b> Account:</b></td>
                           <td><a href='$SCRIPT_NAME?admin=ppcmanager&action=edit_my_account' target='main'>Settings</a></td>
                           <td><a href='$SCRIPT_NAME?admin=ppcmanager&action=withdraw' target='main'>Withdraw Funds</a></td>
                         </tr>
                      </table>

                   </fieldset>
               </body>
               </html>
            ";
						exit;
				}



				// --------------------------------------------------------------------- //
				//  List deposits, by account, by month
				// --------------------------------------------------------------------- //
				function deposit_report ()
				{
						global $SCRIPT_NAME;
						global $config;
						global $language;
						global $CGI;
						global $oSQL;
						global $my_accounts;
						global $PHP_AUTH_USER;
						$this_year = date ( "Y" );
						if ( $CGI[ 'year' ] ) {
								$this_year = $CGI[ 'year' ];
						}
						$year_options = '';
						for ( $x = 2004; $x <= 2020; $x++ ) {
								$selected = '';
								if ( $x == $this_year ) {
										$selected = "selected";
								}
								$year_options .= "
                  <option value='$x' $selected>$x</option>
                ";
						}
						if ( empty( $CGI[ 'month' ] ) ) {
								$CGI[ 'month' ] = date ( "m" );
						}
						$form   = "

               <form action='$SCRIPT_NAME' method='post'>

               Month:
               <select name='month'>
                  <option value='$CGI[month]' selected>$CGI[month]</option>
                  <option value='01'>01</option>
                  <option value='02'>02</option>
                  <option value='03'>03</option>
                  <option value='04'>04</option>
                  <option value='05'>05</option>
                  <option value='06'>06</option>
                  <option value='07'>07</option>
                  <option value='08'>08</option>
                  <option value='09'>09</option>
                  <option value='10'>10</option>
                  <option value='11'>11</option>
                  <option value='12'>12</option>
               </select>

               Year: <select name='year'>
                 $year_options
               </select>

               <input type='hidden' name='action' value='deposit_report'>
               <input type='hidden' name='admin' value='ppcmanager'>
               <input type='hidden' name='acct_id' value='$CGI[acct_id]'>
               <input type='submit' value='Run Report'>

               </form>

            ";
						$report = '';
						if ( !empty( $CGI[ 'month' ] ) && !empty( $CGI[ 'year' ] ) ) {
								$total_deposits   = 0;
								$total_commission = 0;
								$month            = $CGI[ 'month' ];
								$year             = $CGI[ 'year' ];
								$start            = $year . $month . "01";
								//$num_days = cal_days_in_month(CAL_GREGORIAN, $month, $year);
								$num_days  = 31;
								$end       = $year . $month . $num_days;
								$starttime = mktime ( 0 , 0 , 0 , $month , "01" , $year );
								$endtime   = mktime ( 0 , 0 , 0 , $month , $num_days , $year );
								$report    = "<b>Deposits: $month/$year</b>";
								$list      = join ( "','" , array_keys ( $my_accounts ) );
								if ( $CGI[ 'acct_id' ] ) {
										$SQL = "SELECT * FROM " . TRANSACTIONS_TABLE . " WHERE acct_id='$CGI[acct_id]' AND trans_date BETWEEN $starttime AND $endtime AND trans_type = 'CASH' ORDER BY trans_date ASC";
								} else {
										$SQL = "SELECT * FROM " . TRANSACTIONS_TABLE . " WHERE acct_id IN('$list') AND trans_date BETWEEN $starttime AND $endtime AND trans_type = 'CASH' ORDER BY trans_date ASC";
								}
								$deposits = $oSQL->search ( array ( "SQL" => $SQL ) );
								$output   = '';
								foreach ( $deposits as $deposit ) {
										$fdate = date ( "m/d/Y h:i" , $deposit[ 'trans_date' ] );
										if ( preg_match ( "/cash/i" , $deposit[ 'trans_type' ] ) && $deposit[ 'acct_id}' ] == $PHP_AUTH_USER ) {
												continue;
										}
										$total_deposits += $deposit[ 'trans_amount' ];
										$total_commission += $deposit[ 'commission_amount' ];
										$output .= "
                     <tr>
                       <td>$fdate</td>
                       <td>$deposit[acct_id]</td>
                       <td align='right'><b>$language[dollar_sign] $deposit[trans_amount]</b></td>
                       <td align='right'>$deposit[trans_type]</td>
                       <td><i>$deposit[source]</i></td>
                     </tr>
                   ";
								}
								$total_deposits   = sprintf ( "%5.2f" , $total_deposits );
								$total_commission = sprintf ( "%5.2f" , $total_commission );
								$report .= "
                   <link rel='stylesheet' type='text/css' href='$config[script_dir]/tablesort.css'>
                   <table cellspacing=4 class='sorted'>
                     <thead>
                     <tr bgcolor='#e0e0e0'>
                       <td>Date</td>
                       <td>Account</td>
                       <td>Amount</td>
                       <td>Type</td>
                       <td>Source</td>
                     </tr>
                     </thead>
                     <tbody>
                     $output

                     </tbody>

                     <tr>
                       <td>total cash deposits</td>
                       <td colspan=4 align='right'>
                           <i>$language[dollar_sign] $total_deposits</i>
                       </td>
                     </tr>

                     <tr>
                       <td>total commission</td>
                       <td colspan=4 align='right'>
                           <i>$language[dollar_sign] $total_commission</i>
                       </td>
                     </tr>

                   </table>
                ";
						}
						admin_output ( "Deposit Report" , $form . $report );
				}


				// --------------------------------------------------------------------- //
				//  Summarize Earnings for all time
				// --------------------------------------------------------------------- //
				function earnings_report ()
				{
						global $SCRIPT_NAME;
						global $config;
						global $language;
						global $CGI;
						global $oSQL;
						global $my_accounts;
						global $PHP_AUTH_USER;
						$report         = '';
						$total_deposits = 0;
						$total_paid     = 0;
						$total_earned   = 0;
						$num_deposits   = 0;
						$months         = array ();
						$report         = "<b>Earnings/Payments YTD</b>";
						$list           = join ( "','" , array_keys ( $my_accounts ) );
						$SQL            = "SELECT * FROM " . TRANSACTIONS_TABLE . " WHERE acct_id IN('$list') AND trans_type = 'CASH' ORDER BY trans_date ASC";
						$deposits       = $oSQL->search ( array ( "SQL" => $SQL ) );
						$output         = '';
						foreach ( $deposits as $deposit ) {
								$fdate                    = date ( "m/d/Y h:i" , $deposit[ 'trans_date' ] );
								$mon                      = date ( "Ym" , $deposit[ 'trans_date' ] );
								$months[ $mon ][ 'date' ] = date ( "M, Y" , $deposit[ 'trans_date' ] );
								$m                        = date ( "m" , $deposit[ 'trans_date' ] );
								$y                        = date ( "y" , $deposit[ 'trans_date' ] );
								$months[ $mon ][ 'link' ] = $SCRIPT_NAME . "?admin=ppcmanager&action=deposit_report&month=$m&year=$y";
								if ( preg_match ( "/cash/i" , $deposit[ 'trans_type' ] ) && $deposit[ 'acct_id}' ] == $PHP_AUTH_USER ) {
										continue;
								}
								$months[ $mon ][ 'total_deposits' ] += $deposit[ 'trans_amount' ];
								$months[ $mon ][ 'num_deposits' ]++;
								if ( $deposit[ 'commission_paid_date' ] >= 1 ) {
										$months[ $mon ][ 'total_paid' ] += $deposit[ 'commission_amount' ];
										$total_paid += $deposit[ 'commission_amount' ];
								}
								$months[ $mon ][ 'total_earned' ] += $deposit[ 'commission_amount' ];
								$total_deposits += $deposit[ 'trans_amount' ];
								$total_earned += $deposit[ 'commission_amount' ];
								$num_deposits++;
						}
						foreach ( $months as $mon => $info ) {
								$total  = sprintf ( "%5.2f" , $months[ $mon ][ 'total_deposits' ] );
								$earned = sprintf ( "%5.2f" , $months[ $mon ][ 'total_earned' ] );
								$paid   = sprintf ( "%5.2f" , $months[ $mon ][ 'total_paid' ] );
								$count  = $months[ $mon ][ 'num_deposits' ];
								$date   = $months[ $mon ][ 'date' ];
								$link   = $months[ $mon ][ 'link' ];
								$due    = sprintf ( "%5.2f" , $earned - $paid );
								$output .= "
                   <tr>
                     <td><a href='$link' target='main'>$date</a></td>
                     <td>$count</td>
                     <td>$total</td>
                     <td>$earned</td>
                     <td>$paid</td>
                     <td>$due</td>
                   </tr>
                ";
						}
						$total_deposits = sprintf ( "%5.2f" , $total_deposits );
						$total_paid     = sprintf ( "%5.2f" , $total_paid );
						$total_earned   = sprintf ( "%5.2f" , $total_earned );
						$due            = sprintf ( "%5.2f" , $total_earned - $total_paid );
						$report .= "
               <link rel='stylesheet' type='text/css' href='$config[script_dir]/tablesort.css'>
               <table cellspacing=4 class='sorted'>
                 <thead>
                 <tr bgcolor='#e0e0e0'>
                   <td>Month</td>
                   <td># Deposits</td>
                   <td>Total Deposits</td>
                   <td>Total Earned</td>
                   <td>Total Paid</td>
                   <td>Net Due</td>
                 </tr>
                 </thead>
                 <tbody>

                 $output

                 </tbody>

                 <tr>
                   <td><b>YTD</b></td>
                   <td><i>$num_deposits</i></td>
                   <td><i>$language[dollar_sign] $total_deposits</i></td>
                   <td><i>$language[dollar_sign] $total_earned</i></td>
                   <td><i>$language[dollar_sign] $total_paid</i></td>
                   <td><i>$language[dollar_sign] $due</i></td>
                 </tr>

               </table>
            ";
						admin_output ( "Earnings Report" , $report );
				}


				// --------------------------------------------------------------------- //
				//  Quick and dirty click report
				// --------------------------------------------------------------------- //
				function click_report ()
				{
						global $CGI;
						global $language;
						global $config;
						global $oSQL;
						global $my_accounts;
						$form         = $this->rpt_form ();
						$REPORT       = '';
						$LINE_ITEMS   = '';
						$starttime    = $CGI[ 'start_year' ] . $CGI[ 'start_month' ] . $CGI[ 'start_day' ];
						$endtime      = $CGI[ 'end_year' ] . $CGI[ 'end_month' ] . $CGI[ 'end_day' ];
						$start_string = $CGI[ 'start_month' ] . "/" . $CGI[ 'start_day' ] . "/" . $CGI[ 'start_year' ];
						$end_string   = $CGI[ 'end_month' ] . "/" . $CGI[ 'end_day' ] . "/" . $CGI[ 'end_year' ];
						##  Accounts ##
						$accounts = '';
						$accts    = array ();
						if ( $CGI[ 'acct_id' ] ) {
								$list = $CGI[ 'acct_id' ];
						} else {
								$list = join ( "','" , array_keys ( $my_accounts ) );
						}
						if ( !empty( $CGI[ 'run' ] ) && $list ) {
								$num_accounts    = count ( $my_accounts );
								$SQL             = "SELECT * FROM " . CLICK_SUMMARY_TABLE . " WHERE click_date BETWEEN $starttime AND $endtime AND acct_id IN('$list')";
								$data            = $oSQL->search ( array ( "SQL" => $SQL ) );
								$account_summary = array ();
								$date_summary    = array ();
								$total_clicks    = 0;
								$total_fraud     = 0;
								$total_revenue   = 0;
								foreach ( $data as $hit ) {
										$date                            = $hit[ 'click_date' ];
										$show_date                       = formatdate ( $date );
										$count                           = $hit[ 'click_count' ];
										$amt                             = $hit[ 'click_amount_gross' ];
										$num_fraud                       = $hit[ 'fraud_count' ];
										$acct                            = $hit[ 'acct_id' ];
										$date_summary[ $date ][ 'date' ] = $show_date;
										$date_summary[ $date ][ 'ttl_clicks' ] += $count;
										$date_summary[ $date ][ 'ttl_revenue' ] += $amt;
										$date_summary[ $date ][ 'ttl_fraud' ] += $num_fraud;
										$account_summary[ $date ][ $acct ][ 'date' ] = $show_date;
										$account_summary[ $date ][ $acct ][ 'ttl_clicks' ] += $count;
										$account_summary[ $date ][ $acct ][ 'ttl_revenue' ] += $amt;
										$account_summary[ $date ][ $acct ][ 'ttl_fraud' ] += $num_fraud;
										$total_clicks += $count;
										$total_fraud += $num_fraud;
										$total_revenue += $amt;
								}
								ksort ( $date_summary );
								foreach ( $date_summary as $date => $info ) {
										## Breakdown each day by account
										if ( empty( $CGI[ 'acct_id' ] ) && $num_accounts >= 1 ) {
												## Date Header
												$LINE_ITEMS .= "
                           <tr>
                              <td colspan='4'>
                                 <b>$info[date]</b>
                              </td>
                           </tr>
                       ";
												foreach ( $account_summary[ $date ] as $acct => $acctinfo ) {
														$LINE_ITEMS .= "
                               <tr>
                                  <td align='right'>
                                     <i>$acct</i>
                                  </td>
                                  <td align='right'>
                                     $acctinfo[ttl_fraud]
                                  </td>
                                  <td align='right'>
                                     $acctinfo[ttl_clicks]
                                  </td>
                                  <td align='right'>
                                     $language[dollar_sign]$acctinfo[ttl_revenue]
                                  </td>
                               </tr>
                           ";
												}
												## Date Total
												$LINE_ITEMS .= "
                           <tr>
                              <td align='right'>
                                 Sub Total:
                              </td>
                              <td align='right'>
                                 $info[ttl_fraud]
                              </td>
                              <td align='right'>
                                 $info[ttl_clicks]
                              </td>
                              <td align='right'>
                                 $language[dollar_sign]$info[ttl_revenue]
                              </td>
                           </tr>
                       ";
										} ## Just the totals
										else {
												$LINE_ITEMS .= "
                           <tr>
                              <td>
                                 <b>$info[date]</b>
                              </td>
                              <td align='right'>
                                 $info[ttl_fraud]
                              </td>
                              <td align='right'>
                                 $info[ttl_clicks]
                              </td>
                              <td align='right'>
                                 $language[dollar_sign]$info[ttl_revenue]
                              </td>
                           </tr>
                       ";
										}
								}
								$REPORT = "
                   <link rel='stylesheet' href='$config[include_dir]/tablesort.css'>
                   <br>
                   <table class='sorted'>
                      <thead>
                      <tr>
                        <td>$language[date_header]</td>
                        <td>$language[fraud_count]</td>
                        <td>$language[total_clicks]</td>
                        <td>$language[total_revenue]</td>
                      </tr>
                      </thead>
                      <tbody>

                       $LINE_ITEMS

                       <tr>
                          <td style='border-top:1px solid silver'>
                             <b>$language[report_total]</b>
                          </td>
                          <td align='right' style='border-top:1px solid silver'>
                             $total_fraud
                          </td>
                          <td align='right' style='border-top:1px solid silver'>
                             $total_clicks
                          </td>
                          <td align='right' style='border-top:1px solid silver'>
                             $language[dollar_sign]$total_revenue
                          </td>
                       </tr>

                      </tbody>
                   </table>
               ";
						}
						admin_output ( "Click Report" , "<center>$form $REPORT</center>" );
				}


				// --------------------------------------------------------------------- //
				//  Draw the report form
				// --------------------------------------------------------------------- //
				function rpt_form ()
				{
						global $CGI;
						global $SCRIPT_NAME;
						global $language;
						$day       = date ( "d" );
						$month     = date ( "m" );
						$year      = date ( "Y" );
						$next_year = $year + 1;
						$last_year = $year - 1;
						if ( $CGI[ 'start_month' ] ) {
								$start_month_option = "<option value='$CGI[start_month]'>$CGI[start_month]</option>";
						}
						if ( $CGI[ 'end_month' ] ) {
								$end_month_option = "<option value='$CGI[end_month]'>$CGI[end_month]</option>";
						}
						$month_options = "<option value='$month'>$month</option>\n";
						for ( $m = 1; $m <= 12; $m++ ) {
								$x = $m;
								$x = sprintf ( "%02d" , $x );
								$month_options .= "<option value='$x'>$x</option>\n";
						}
						if ( $CGI[ 'start_day' ] ) {
								$start_day_option = "<option value='$CGI[start_day]'>$CGI[start_day]</option>";
						}
						if ( $CGI[ 'end_day' ] ) {
								$end_day_option = "<option value='$CGI[end_day]'>$CGI[end_day]</option>\n";
						}
						$day_options = "<option value='$day'>$day</option>\n";
						for ( $d = 1; $d <= 31; $d++ ) {
								$x = $d;
								$x = sprintf ( "%02d" , $x );
								$day_options .= "<option value='$x'>$x</option>\n";
						}
						if ( $CGI[ 'start_year' ] ) {
								$start_year_option = "<option value='$CGI[start_year]'>$CGI[start_year]</option>";
						}
						if ( $CGI[ 'end_year' ] ) {
								$end_year_option = "<option value='$CGI[end_year]'>$CGI[end_year]</option>\n";
						}
						$year_options = "
             <option value='$year'>$year</option>
             <option value='$last_year'>$last_year</option>
             <option value='$next_year'>$next_year</option>
           ";
						if ( !$CGI[ 'start_month' ] ) {
								$CGI[ 'start_month' ] = $mon;
								$CGI[ 'start_year' ]  = $year;
								$CGI[ 'start_day' ]   = $mday;
						}
						if ( !$CGI[ 'end_month' ] ) {
								$CGI[ 'end_month' ] = $mon;
								$CGI[ 'end_year' ]  = $year;
								$CGI[ 'end_day' ]   = $mday;
						}
						$account_input = "";
						if ( !empty( $CGI[ 'acct_id' ] ) ) {
								$account_input = "
                  Account: $CGI[acct_id] <input type='hidden' name='acct_id' value='$CGI[acct_id]' size='8' />
               ";
						}

						return "

              <form name='report_form' action='$SCRIPT_NAME' method='get' target='main'>
                  <table border=0>

                  <tr>
                    <td colspan='4' norwap='nowrap'>
                        $account_input
                    </td>
                  </tr>

                  <tr>
                     <td>$language[report_start_date]</td>
                     <td>
                        <select name='start_month'>
                           $start_month_option
                           $month_options
                        </select>
                     </td>
                     <td>
                        <select name='start_day'>
                           $start_day_option
                           $day_options
                        </select>
                     </td>
                     <td>
                        <select name='start_year'>
                           $start_year_option
                           $year_options
                        </select>
                     </td>
                  </tr>

                  <tr>
                     <td>$language[report_end_date]</td>
                     <td>
                        <select name='end_month'>
                           $end_month_option
                           $month_options
                        </select>
                     </td>
                     <td>
                        <select name='end_day'>
                           $end_day_option
                           $day_options
                        </select>
                     </td>
                     <td>
                        <select name='end_year'>
                           $end_year_option
                           $year_options
                        </select>
                     </td>
                  </tr>

                  <tr>
                    <td colspan=4>
                     <input type='hidden' name='action' value='click_report'>
                     <input type='hidden' name='admin' value='ppcmanager'>
                     <input type='hidden' name='run' value='1'>
                     <center><input type='submit' value='Run Report'></center>
                  </tr>
                  </table>
              </form>
           ";
				}


				// --------------------------------------------------------------------- //
				//  Withdraw funds from account
				// --------------------------------------------------------------------- //
				function withdraw ()
				{
						global $iSQL;
						global $CGI;
						global $language;
						global $config;
						global $PHP_AUTH_USER;
						global $SCRIPT_NAME;
						$SQL           = "SELECT * FROM " . ACCOUNTS_TABLE . " WHERE acct_id = '$PHP_AUTH_USER'";
						$account       = $iSQL->search ( array ( "SQL" => $SQL , "Return" => "hash" ) );
						$message       = "";
						$balance       = $account[ 'current_balance' ];
						$avail_balance = sprintf ( "%5.2f" , $balance * .80 );
						$name          = $account[ 'contact_name' ];
						$amount        = $CGI[ 'amount' ];
						if ( $amount ) {
								$amount = preg_replace ( "/\$/" , "" , $amount );
								$amount = sprintf ( "%5.2f" , $amount );
								if ( $amount > $avail_balance ) {
										$ERROR = "Invalid amount.  $amount is more than your available balance of $avail_balance";
								} else {
										$wd                             = new Record( array ( "Parent" => $iSQL , "Table" => WITHDRAWLS_TABLE , "Schema" => WITHDRAWLS_TABLE , "Lookup" => 1 , "acct_id" => $PHP_AUTH_USER ) );
										$wd->data[ 'acct_id' ]          = $PHP_AUTH_USER;
										$wd->data[ 'withdrawl_amount' ] = $amount;
										$wd->update_values ( $wd->data );
										$wd->save ();
										mail ( $config[ 'email' ] , "Withdrawl Request: $PHP_AUTH_USER" , "$PHP_AUTH_USER has requested a withdrawl of $amount from their account" , "From: $config[email]\r\n" );
										$ERROR = "Withdrawl Request for $amount sent to administrator";
								}
						} else {
								$SQL             = "SELECT withdrawl_amount FROM " . WITHDRAWLS_TABLE . " WHERE acct_id = '$PHP_AUTH_USER'";
								$existing_amount = $iSQL->search ( array ( "SQL" => $SQL , "Return" => "single" ) );
								if ( $existing_amount ) {
										$ERROR = "
                       <i>You already have a withdrawl of $language[dollar_sign] $existing_amount pending.
                       If you enter a new amount here, the original request will be vacated, and replaced by
                       this one.</i><br /><br />
                   ";
								}
						}
						$message = "
               <b>Withdrawl Request for $name ($PHP_AUTH_USER)</b><br /><br />
               <b>Current Balance:</b> $balance<br />
               <b>Available Balance (80%):</b> $avail_balance<br /><br />

               <span style='color:red'>$ERROR</span>

               <form action='$SCRIPT_NAME' method='post'>
                   Withdrawl Amount: <input name='amount' />
                     <input type='submit' value='Make Withdrawl' />
                     <input type='hidden' name='action' value='withdraw' />
                     <input type='hidden' name='admin' value='ppcmanager' />
                   <br />
                   Please enter your withdrawl amount in dd.cc format.  Do not use
                   a dollar sign.
               </form>
           ";
						admin_output ( "Withdrawl" , $message );
				}


				// --------------------------------------------------------------------- //
				//  Edit account record
				// --------------------------------------------------------------------- //
				function edit_my_account ()
				{
						global $CGI;
						global $PHP_AUTH_USER;
						$CGI[ 'acct_id' ] = $PHP_AUTH_USER;
						$this->view_account ();
				}

				function view_account ()
				{
						global $CGI;
						global $oSQL;
						global $language;
						global $PHP_AUTH_USER;
						$record = new Record( array ( "Parent" => $oSQL , "Table" => ACCOUNTS_TABLE , "Schema" => ACCOUNTS_TABLE , "Lookup" => 1 , "acct_id" => $CGI[ 'acct_id' ] ) );
						if ( !$record->data[ create_date ] ) {
								$record->data[ create_date ] = date ( "Ymd" );
						}
						$formarray  = $record->generate_form ();
						$formfields = '';
						while ( list ( $prompt , $fieldinput ) = each ( $formarray ) ) {
								$formfields .= "<tr bgcolor='#e0e0e0'><td>$prompt</td><td>$fieldinput</td></tr>\n";
						}
						$name        = $record->data[ contact_name ];
						$SQL         = "SELECT COUNT(*) FROM " . PENDING_BIDS_TABLE . " WHERE account = '$CGI[acct_id]'";
						$num_pending = $oSQL->search ( array ( "SQL" => $SQL , "Return" => "single" ) );
						$menu        = '';
						if ( $CGI[ 'acct_id' ] != $PHP_AUTH_USER ) {
								$menu = "
                <B>Admin:</B>
                <B><A HREF='$SCRIPT_NAME?admin=ppcmanager&action=manage_bids&acct_id=$CGI[acct_id]'>Manage Current Bids</A></B> /
                <B><A HREF='$SCRIPT_NAME?admin=ppcmanager&action=pending_bids&account=$CGI[acct_id]'>Pending Bids</A> ($num_pending)</B> /
                <B><A HREF='$SCRIPT_NAME?admin=ppcmanager&action=click_report&acct_id=$CGI[acct_id]'>Financial Report</A></B> /
                <B><A HREF='$SCRIPT_NAME?admin=ppcmanager&action=deposit_report&acct_id=$CGI[acct_id]'>Deposit History</A></B>
               ";
						}
						$output = "
                <br />
                <blockquote>
                  <form action='$SCRIPT_NAME' method='post'>
                  <table cellpadding=8 cellspacing=0 border=0>
                    <tr bgcolor='navy'><td colspan=2>
                      <font color='yellow'>
                         $language[edit_acct_title] <b>$name</b>
                      </font>
                    </td></tr>

                    <tr bgcolor='#efefef'>
                      <td style='border:1px outset silver' colspan='2'>
                         $menu
                      </td>
                    </tr>


                    $formfields
                    <tr><td colspan=2>
                       <center>
                         <select name='action'>
                            <option value='save_account'>$language[save_button]</option>
                            <option value='delete_account'>$language[delete_button]</option>
                         </select>
                         <input type='submit' value='Go'>
                       </center>
                    </td></tr>
                   </table>
                   <input type='hidden' name='admin' value='ppcmanager' />
                 </form>
                 </blockquote>
           ";
						admin_output ( "Edit $name" , $output );
				}



				// --------------------------------------------------------------------- //
				//  Save Account Record
				// --------------------------------------------------------------------- //
				function save_account ()
				{
						global $iSQL;
						global $CGI;
						global $language;
						$record = new Record( array ( "Parent" => $iSQL , "Table" => ACCOUNTS_TABLE , "Schema" => ACCOUNTS_TABLE , "Lookup" => 1 , "acct_id" => $CGI[ acct_id ] ) );
						if ( $record->update_values ( $CGI ) ) {
								if ( $record->save () ) {
										$message = $language[ record_saved ];
								} else {
										$message = "$language[record_save_error]<pre>$record->error_text</pre>";
								}
						} else {
								$message = "$language[record_save_error]<pre>$record->error_text</pre>";
						}
						admin_output ( "$language[status_box_header]" , $message );
				}


				// --------------------------------------------------------------------- //
				//  Delete Account Record
				// --------------------------------------------------------------------- //
				function delete_account ()
				{
						global $iSQL;
						global $CGI;
						global $language;
						$record = new Record( array ( "Parent" => $iSQL , "Table" => ACCOUNTS_TABLE , "Schema" => ACCOUNTS_TABLE , "Lookup" => 1 , "acct_id" => $CGI[ acct_id ] ) );
						if ( $record->delete () ) {
								$message = "Account $CGI[acct_id] deleted";
						} else {
								$message = "Delete Error: $record->error_text";
						}
						admin_output ( "$language[status_box_header]" , $message );
				}



				// --------------------------------------------------------------------- //
				//  Mass eMailing form
				// --------------------------------------------------------------------- //
				function massmail ()
				{
						global $iSQL;
						global $CGI;
						global $language;
						global $my_accounts;
						global $SCRIPT_NAME;
						$acctlist = '';
						foreach ( $my_accounts as $account => $name ) {
								$acctlist .= " <option value='$account'>$name ($account)</option> ";
						}
						$form = "
               <form action='$SCRIPT_NAME' method='POST'>

               <table width='500' summary='emailform' cellspacing='5'>
                 <tr>
                   <td nowrap='nowrap'>
                      Which Accounts ?
                   </td>
                   <td nowrap='nowrap'>
                      <select name='acct_type'>
                          <option value='all'>All Accounts</option>
                          <option value='low_balance'>Low Balances (less than \$10.00)</option>
                          <option value='no_balance'>No Balance ( \$0.00 or less)</option>
                          $acctlist
                      </select>
                   </td>
                 </tr>
                 <tr>
                   <td nowrap='nowrap'>
                      Email Subject:
                   </td>
                   <td nowrap='nowrap'>
                      <input name='subject' size='50' />
                   </td>
                 </tr>
                 <tr>
                   <td colspan='2' nowrap='nowrap'>
                     Email Text:<br />
                     <textarea cols='60' rows='10' name='email_text'></textarea>
                   </td>
                 </tr>
                 <tr>
                   <td colspan='2' align='center'>
                       <input type='hidden' name='action' value='send_massmail' />
                       <input type='hidden' name='admin' value='ppcmanager' />
                       <input type='submit' value='Send Email to all Accounts' />
                   </td>
                 </tr>
                 <tr>
                   <td valign='top' colspan='2'>
                      <hr />
                      Note that you may use any of the following variables
                      within your email to merge users' information into
                      your message:
                      <br /><br />
                      <b>&lt;&lt;LOGIN&gt;&gt;</b>: Login Id<br />
                      <b>&lt;&lt;PASSWORD&gt;&gt;</b>: Password<br />
                      <b>&lt;&lt;BALANCE&gt;&gt;</b>: Account Balance<br />
                      <b>&lt;&lt;NAME&gt;&gt;</b>: Full Name<br />
                   </td>
                 </tr>
               </table>

               </form>

           ";
						admin_output ( "Prepare Email" , $form );
				}


				// --------------------------------------------------------------------- //
				//  Send Mass eMail
				// --------------------------------------------------------------------- //
				function send_massmail ()
				{
						global $oSQL;
						global $CGI;
						global $language;
						global $my_accounts;
						global $PHP_AUTH_USER;
						ob_end_flush ();
						$record     = new Record( array ( "Parent" => $oSQL , "Table" => ACCOUNTS_TABLE , "Schema" => ACCOUNTS_TABLE , "Lookup" => 1 , "acct_id" => $PHP_AUTH_USER ) );
						$email_text = $CGI[ 'email_text' ];
						$subject    = $CGI[ 'subject' ];
						$from       = $record->data[ 'contact_email' ];
						$in         = join ( "','" , array_keys ( $my_accounts ) );
						$where      = "";
						if ( $CGI[ 'acct_type' ] == "all" ) {
								$where = "WHERE acct_id IN('$in')";
						} elseif ( $CGI[ 'acct_type' ] == "low_balance" ) {
								$where = "WHERE acct_id IN('$in') AND current_balance BETWEEN 0.00 AND 10.00";
						} elseif ( $CGI[ 'acct_type' ] == "no_balance" ) {
								$where = "WHERE acct_id IN('$in') AND current_balance <= 0.00";
						} else {
								$where = "WHERE acct_id = '$CGI[acct_type]'";
						}
						$SQL      = "SELECT * FROM " . ACCOUNTS_TABLE . " " . $where;
						$accounts = $oSQL->search ( array ( "SQL" => $SQL ) );
						foreach ( $accounts as $account ) {
								$account[ 'balance' ]  = $account[ 'current_balance' ];
								$account[ 'login' ]    = $account[ 'acct_id' ];
								$account[ 'password' ] = $account[ 'acct_password' ];
								$account[ 'name' ]     = $account[ 'contact_name' ];
								$to                    = $account[ 'contact_email' ];
								## Create and merge the template with the listing data.
								$template  = new SubTemplate( array ( "front" => "<<" , "back" => ">>" , "template_text" => $email_text , "values" => $account ) );
								$mail_body = $template->merge ();
								mail ( $to , $subject , $mail_body , "From: $from\r\n" );
								echo "$to<br />\n";
								flush ();
						}
						echo "<br /><br /><b>Operation Complete</b>";
						exit;
				}
		} // End Class


?>
