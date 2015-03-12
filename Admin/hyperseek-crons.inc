<?php

		/*
		________________________________________________________________________________

		     	PROGRAM : Hyperseek Admin Cron Functions
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

			   	Valid Hyperseek Cron Command Lines ....

							Re-Count Categories:
											admin.php -cron hyperseek category_count

							Monthly Keyword Billing:
											admin.php -cron hyperseek monthlybilling {charge}

							Verify Active URLs:
											admin.php -cron hyperseek deadlinks {n}

							Pay affiliates (direct deposit into account):
											admin.php -cron hyperseek affpay

							Audit Affiliates:
											admin.php -cron hyperseek audit

							Autobill Accounts:
											admin.php -cron hyperseek autobill

							Re-Generate Master Merge Tables:
											admin.php -cron hyperseek mergetables

							Re-Summarize all Clicks & Fraud:
											admin.php -cron hyperseek summarize_clicks {date}
											Note that date can be "all" or a date in "yyyymmdd" format

							Clean Up Jackhammer Fraud Log Tables:
											admin.php -cron hyperseek table_cleanup

							Re-Generate Related Keywords List:
											admin.php -cron hyperseek generate_related_keywords

							Nuke Database
											admin.php -cron hyperseek nuke

							Nuke Click Stats
											admin.php -cron hyperseek nuke_stats

							Import Category Tree:
											admin.php -cron hyperseek import_cats {file}

							Import Listings
											admin.php -cron hyperseek import_links {file}

							Import links into the queue
											admin.php -cron hyperseek import_into_queue {file}

							Check Daily Limits and reset links and accounts
											admin.php -cron hyperseek daily_limits
      ________________________________________________________________________________
   */


		// Re-Count the categories
		function cron_category_count ()
		{
				$hyperseek = new hyperseekAdmin();
				$hyperseek->category_count ();
				exit;
		}


		// Verify Listing URls
		function cron_deadlinks ()
		{
				global $iSQL;
				global $CGI;
				global $language;
				global $config;
				global $my_accounts;
				global $SCRIPT_NAME;
				global $argv;
				$batch = $argv[ 4 ];
				if ( !$batch ) {
						$batch = 100;
				}
				$count = 0;
				echo "Checking Listings .... Batch Size: $batch \n";
				$SQL           = "SELECT id FROM " . VALIDATE_TABLE;
				$last_verified = $iSQL->search ( array ( "SQL" => $SQL , "Return" => "single" ) );
				if ( !$last_verified ) {
						$last_verified = 0;
				}
				$SQL = "DELETE FROM " . VALIDATE_TABLE;
				$iSQL->run ( $SQL );
				$SQL = "INSERT INTO " . VALIDATE_TABLE . " VALUES(0)";
				$iSQL->run ( $SQL );
				$SQL   = "SELECT id, url FROM " . LINKS_TABLE . " WHERE id > $last_verified ORDER BY id limit $batch";
				$links = $iSQL->search ( array ( "SQL" => $SQL ) );
				foreach ( $links as $listing ) {
						$code = check_url ( $listing[ 'url' ] );
						if ( $code == 200 ) {
								echo "+";
						} else {
								echo "-";
						}
						echo " $listing[url] : [$code]\n";
						$report                      = new Record( array ( "Parent" => $iSQL , "Table" => LINKS_TABLE , "Schema" => LINKS_TABLE , "Lookup" => 1 , "id" => $listing[ id ] ) );
						$report->data[ last_status ] = $code;
						$report->update_values ( $report->data );
						$report->save ();
						$SQL = "UPDATE " . VALIDATE_TABLE . " SET id = $listing[id]";
						$iSQL->run ( $SQL );
						$count++;
				}
				if ( $count < $batch ) {
						$SQL = "UPDATE " . VALIDATE_TABLE . " SET id = 0";
						$iSQL->run ( $SQL );
						echo $language{verify_done};
				} else {
						echo $language{verify_batch_done};
				}
				exit;
		}

		function check_url ( $url = '' )
		{
				$ch = curl_init ( $url );
				curl_setopt ( $ch , CURLOPT_HEADER , 1 );
				curl_setopt ( $ch , CURLOPT_NOBODY , 1 );
				curl_setopt ( $ch , CURLOPT_RETURNTRANSFER , 1 );
				curl_setopt ( $ch , CURLOPT_TIMEOUT , 5 );
				$header = curl_exec ( $ch );
				$header = preg_replace ( "/\cM|\n|\r/" , " " , $header );
				preg_match ( "/http(.*?)\s+(.*?)\s+/i" , $header , $m );

				return trim ( $m[ 2 ] );
		}

		// Pay Affiliates -- deposit earnings into their account
		function cron_affpay ()
		{
				global $SCRIPT_NAME;
				global $language;
				global $iSQL;
				global $CGI;
				global $config;
				$final_report = '';
				$grand_total  = '';
				$rows         = '';
				$accounts     = array ();
				$pay          = array ();
				$paylist      = array ();
				$timestamp    = time ();
				$yesterday    = time () - ( 24 * 3600 );
				$merge_date   = date ( "Ym" , $yesterday );
				$mon          = date ( "m" , $yesterday );
				$day          = date ( "d" , $yesterday );
				$year         = date ( "Y" , $yesterday );
				$period_start = $period_end = date ( "Ymd" , $yesterday );
				$paid_date    = date ( "m/d/Y" , $yesterday );
				if ( $CGI[ acct_id ] ) {
						array_push ( $accounts , $CGI[ acct_id ] );
				} else {
						$SQL      = "SELECT affiliate FROM " . AFF_INFO_TABLE . " WHERE payment_type = 'deposit'";
						$accounts = $iSQL->search ( array ( "SQL" => $SQL , "Return" => "array" ) );
				}
				echo "Date: $mon/$day/$year\n\n";
				foreach ( $accounts as $affiliate ) {
						echo "Working on: $affiliate ... ";
						if ( !$affiliate ) {
								continue;
						}
						$account = new Account( array ( "Parent" => $iSQL , "Table" => ACCOUNTS_TABLE , "Schema" => ACCOUNTS_TABLE , "Lookup" => 1 , "acct_id" => $affiliate ) );
						list ( $report , $total , $s , $t ) = $account->generate_affiliate_payment_report ( array ( "end_day" => $day , "end_month" => $mon , "end_year" => $year , "start_day" => $day , "start_month" => $mon , "start_year" => $year , "minimum" => 0.01 ) );
						if ( $total >= 0.01 ) {
								$pay[ $affiliate ] = $total;
								array_push ( $paylist , $affiliate );
						} else {
								echo "Nothing Due, Skipping";
						}
						echo "\n";
						$grand_total += $total;
				}
				foreach ( $pay as $affiliate => $amount ) {
						$payment                       = new Record( array ( "Parent" => $iSQL , "Table" => AFF_PAYMENTS_TABLE , "Schema" => AFF_PAYMENTS_TABLE ) );
						$payment->data[ affiliate ]    = $affiliate;
						$payment->data[ paid_date ]    = $paid_date;
						$payment->data[ paid_amount ]  = $amount;
						$payment->data[ due_amount ]   = $amount;
						$payment->data[ balance ]      = 0.00;
						$payment->data[ period_start ] = $period_start;
						$payment->data[ period_end ]   = $period_end;
						$payment->data[ tstamp ]       = $timestamp;
						$payment->data[ comments ]     = "Auto Deposit: Earnings from $paid_date";
						$payment->update_values ( $payment->data );
						$payment->save ();
						## Update Account Balance
						$SQL = "UPDATE " . ACCOUNTS_TABLE . " SET current_balance = current_balance + $amount WHERE acct_id = '$affiliate'";
						$iSQL->run ( $SQL );
						## Finance Table Record
						$finances                       = new Record( array ( "Parent" => $iSQL , "Table" => TRANSACTIONS_TABLE , "Schema" => TRANSACTIONS_TABLE ) );
						$finances->data[ acct_id ]      = $affiliate;
						$finances->data[ source ]       = $language[ deposit_message_affiliate ];
						$finances->data[ trans_amount ] = $amount;
						$finances->data[ trans_date ]   = $timestamp;
						$finances->data[ trans_type ]   = "PAYMENT";
						$finances->data[ tax_amount ]   = 0.00;
						$finances->data[ affiliate ]    = "";
						$finances->data[ extra_info ]   = "";
						$finances->update_values ( $finances->data );
						$finances->save ();
						## Update the acct_summary Table Record
						$summary                        = new Record( array ( "Parent" => $iSQL , "Table" => ACCT_SUMMARY_TABLE , "Schema" => ACCT_SUMMARY_TABLE , "Lookup" => 1 , "acct_id" => $affiliate ) );
						$new_total                      = $summary->data[ earnings_paid ] + $amount;
						$summary->data[ acct_id ]       = $affiliate;
						$summary->data[ earnings_paid ] = $new_total;
						$summary->update_values ( $summary->data );
						$summary->save ();
						echo "Paid $affiliate: \$$amount\n";
				}
				$acctlist = implode ( "','" , $paylist );
				// Brute Force Table Updates //
				$SQL = "UPDATE " . AFF_SUMMARY_TABLE . " SET paid_flag = 'YES' WHERE acct_id IN('$acctlist') AND summary_date BETWEEN '$period_start' AND '$period_end'";
				$iSQL->run ( $SQL );
				$SQL = "UPDATE " . AFF_SEARCHES_TABLE . " SET paid_flag = 'YES' WHERE acct_id IN('$acctlist') AND search_date BETWEEN '$period_start' AND '$period_end'";
				$iSQL->run ( $SQL );
				if ( $config[ jackhammer_merge_tables ] == "YES" ) {
						$click_table = CLICK_LOG_TABLE . "_" . $merge_date;
				} else {
						$click_table = CLICK_LOG_TABLE;
				}
				$SQL = "UPDATE $click_table SET paid_flag = 'YES' WHERE hit_date BETWEEN '$period_start' AND '$period_end' AND affiliate IN('$acctlist')";
				$iSQL->run ( $SQL );
				echo "\n\nOPERATION COMPLETE \$$grand_total auto-deposited to affiliates\n";
				exit;
		}

		// Audit Affiliates -- Determine if they've been underpaid
		function cron_audit ()
		{
				# 2 Parts to this:
				#
				# 1. Select every paid date range from the payments table
				#    that is not yet paid (ie: paid 7/1/2000 -> 7/14/2000 and 7/16/2000 -> 7-31/2000 should report 7/15/2000 -> 7/15/2000 as a hole)
				#    Check if any amounts should have been due during this period and report it.
				# 2. Total up all $$ owed from go_hits and affiliate_searches for each affiliate
				#    up to the last paid date.
				#    Subtract the total ever paid, and the current owed balance.
				#
				#    The remainder is owed, make sure it's more than 1% and then report it.
				# For both result sets, allow/prompt the admin to make adjustments to the balances
				# reflective of the correct amount, and with a pre-formatted note about it.
				global $iSQL;
				global $CGI;
				global $language;
				global $config;
				global $my_accounts;
				global $SCRIPT_NAME;
				global $data_dir;
				echo "$language[gen_audit_title]\n";
				$rightnow = date ( "m/d/Y" );
				$report   = '';
				// Table Vars
				$accounts    = ACCOUNTS_TABLE;
				$affinfo     = AFF_INFO_TABLE;
				$affsearches = AFF_SEARCHES_TABLE;
				$clicklog    = CLICK_LOG_TABLE;
				$SQL         = "
               SELECT $accounts.*, $affinfo.*
               FROM $accounts, $affinfo
               WHERE $accounts.acct_id = $affinfo.affiliate
       ";
				$affiliates  = $iSQL->search ( array ( "SQL" => $SQL ) );
				foreach ( $affiliates as $affiliate ) {
						$balance              = '';
						$paid                 = 0;
						$last_paid_date       = '';
						$searches_owed        = 0;
						$searches_marked_paid = 0;
						$clicks_owed          = 0;
						$clicks_marked_paid   = 0;
						$affiliate_report     = '';
						$searches_paid        = array ();
						$searches_unpaid      = array ();
						$clicks_paid          = array ();
						$clicks_unpaid        = array ();
						$affiliate_report .= "<b><u>$affiliate[acct_id]</u></b><blockquote>\n";
						## Total up amount owed for searches
						if ( preg_match ( "/search/i" , $affiliate[ payment_trigger ] ) ) {
								$SQL      = "
              SELECT search_count, commission, paid_flag, search_date FROM $affsearches
                WHERE acct_id='$affiliate[acct_id]'
                GROUP BY paid_flag, commission
            ";
								$searches = $iSQL->search ( array ( "SQL" => $SQL ) );
								foreach ( $searches as $search ) {
										if ( $search[ commission ] == "" || $search[ commission ] == "0.00" || $search[ commission ] == "0" ) {
												continue;
										}
										if ( $search[ paid_flag ] == "YES" ) {
												$searches_paid[ $search[ paid_date ] ] += ( $search[ search_count ] * $search[ commission ] );
												$searches_marked_paid += ( $search[ num_searches ] * $search[ commission ] );
										} else {
												$searches_unpaid[ $search[ paid_date ] ] += ( $search[ search_count ] * $search[ commission ] );
										}
										$searches_owed += $search[ search_count ] * $search[ commission ];
								}
								$searches_owed        = sprintf ( "%5.3f" , ( $searches_owed ) );
								$searches_marked_paid = sprintf ( "%5.3f" , ( $searches_marked_paid ) );
								$affiliate_report .= "
               $language[audit_report_searches_owed] $searches_owed<br />
               $language[audit_report_searches_marked_paid] $searches_marked_paid<br />
            ";
								if ( count ( $searches_unpaid ) >= 1 ) {
										$ttl = '';
										$affiliate_report .= "$language[audit_report_unpaid_by_date]<br />\n";
										foreach ( $searches_unpaid as $date => $amt ) {
												$ttl += $amt;
												$sdate = formatdate ( $date );
												$amt   = sprintf ( "%5.3f" , $amt );
												$affiliate_report .= "&nbsp;&nbsp;&nbsp; $sdate: $amt<br />";
										}
										$ttl = sprintf ( "%5.3f" , ( $ttl ) );
										$affiliate_report .= "&nbsp;&nbsp;&nbsp; $language[audit_report_ttl_unpaid_by_date] $ttl<br /><br />";
								}
						} ## end if search
						## Total up amount owed for clicks
						if ( preg_match ( "/click/i" , $affiliate[ payment_trigger ] ) ) {
								$SQL  = "
               SELECT price, commission, paid_flag, hit_date
               FROM $clicklog WHERE
               affiliate='$affiliate[acct_id]'
             ";
								$hits = $iSQL->search ( array ( "SQL" => $SQL ) );
								foreach ( $hits as $hit ) {
										if ( !$hit{commission} ) {
												continue;
										}
										$paid_date = $hit[ hit_date ];
										$curr_owed = '';
										if ( preg_match ( '/%/' , $hit[ commission ] ) ) {
												$comm = $hit[ commission ];
												$comm = preg_replace ( '/%/' , '' , $comm );
												$curr_owed += ( .01 * $comm ) * $hit[ price ];
										} else {
												$curr_owed += $hit[ price ];
										}
										if ( $hit[ paid_flag ] == "YES" ) {
												$clicks_marked_paid += $curr_owed;
												$clicks_paid[ $paid_date ] += $curr_owed;
										} else {
												$clicks_unpaid[ $paid_date ] += $curr_owed;
										}
										$clicks_owed += $curr_owed;
								}
								$clicks_owed        = sprintf ( "%5.3f" , ( $clicks_owed ) );
								$clicks_marked_paid = sprintf ( "%5.3f" , ( $clicks_marked_paid ) );
								$affiliate_report .= "
               $language[audit_report_clicks_owed] $clicks_owed<br />
               $language[audit_report_clicks_marked_paid] $clicks_marked_paid<br />
             ";
								if ( count ( $clicks_unpaid ) >= 1 ) {
										$ttl = '';
										$affiliate_report .= "$language[audit_report_unpaid_by_date]<br />\n";
										foreach ( $clicks_unpaid as $date => $amt ) {
												$sdate = formatdate ( $date );
												$ttl += $amt;
												$amt = sprintf ( "%5.3f" , ( $amt ) );
												$affiliate_report .= "&nbsp;&nbsp;&nbsp; $sdate: $amt<br />";
										}
										$ttl = sprintf ( "%5.3f" , ( $ttl ) );
										$affiliate_report .= "&nbsp;&nbsp;&nbsp; $language[audit_report_ttl_unpaid_by_date] $ttl<br /><br />";
								}
						} ## end if click
						## Get Current Balance / Total actually paid.
						$SQL      = "SELECT tstamp, balance, paid_date, paid_amount FROM " . AFF_PAYMENTS_TABLE . " WHERE affiliate='$affiliate[acct_id]' ORDER BY tstamp DESC";
						$payments = $iSQL->search ( array ( "SQL" => $SQL ) );
						foreach ( $payments as $payment ) {
								$timestamp   = $payment[ tstamp ];
								$b           = $payment[ balance ];
								$lpd         = $payment[ paid_date ];
								$paid_amount = $payment[ paid_amount ];
								if ( $balance == "" ) {
										$balance = $b;
								}
								if ( $last_paid_date == "" ) {
										$last_paid_date = $lpd;
								}
								$paid += $paid_amount;
						}
						$owed         = $clicks_owed + $searches_owed;
						$owed         = sprintf ( "%01.3f" , ( $owed ) );
						$marked_paid  = $clicks_marked_paid + $searches_marked_paid;
						$marked_paid  = sprintf ( "%01.3f" , ( $marked_paid ) );
						$difference   = $owed - ( $paid + $balance );
						$difference   = sprintf ( "%01.3f" , ( $difference ) );
						$diff_rounded = sprintf ( "%01.3f" , ( $difference ) );
						$paid         = sprintf ( "%01.3f" , ( $paid ) );
						$balance      = sprintf ( "%01.3f" , ( $balance ) );
						## Only report if there is an adjustment to make
						if ( $difference > 0.00 ) {
								list ( $m , $d , $y ) = preg_split ( '/\//' , $last_paid_date );
								$lpd = $y . $m . $d;
								$report .= "
                $affiliate_report
                <br />
                $language[audit_report_summary_for] $affiliate[acct_id]:<br />
                <blockquote>
                $language[audit_report_last_paid] $last_paid_date<br />
                $language[audit_report_total_owed] $owed<br />
                $language[audit_report_marked_paid] $marked_paid<br />
                $language[audit_report_balance] $balance<br />
                $language[audit_report_actually_paid] $paid<br />
                $language[audit_report_difference] $difference   <b>($language[audit_report_rounded_act] $diff_rounded)</B><br />
                </blockquote>
                <a href='admin.php?admin=accounts&action=adjust_balance&lpd=$lpd&affiliate=1&acct_id=$affiliate[acct_id]&amount=$diff_rounded&extra_info=Admin+Adjustment'>Adjust This Account</a></b>
                </blockquote>
             ";
						} else {
								$report .= "
                 <b>$affiliate[acct_id]</b>: No Problems<br /><br />
             ";
						}
				} ## End foreach $affiliate
				write_file ( "$data_dir/hyperseek/affiliates/adjust.rpt" , $report );
				echo "$language[audit_report_complete]\n";
		}

		// Deducts from account balances for hyperseek installations that use monthly
		// keyword billing.  Calculates the "rent" for the upcoming month, and debits the account
		function cron_monthlybilling ()
		{
				#--------------------------------------------------------------------------#
				# This MUST run from The Command Line / Telnet or via CRON.
				# Accepts "charge" as a command line paramater to physically do the charging
				# Otherwise, simply prints out a summary
				#--------------------------------------------------------------------------#
				global $iSQL;
				global $CGI;
				global $language;
				global $config;
				global $my_accounts;
				global $SCRIPT_NAME;
				global $templates;
				global $argv;
				$report        = '';
				$do            = $argv[ 4 ];
				$num_days      = mcal_days_in_month ( date ( "n" ) );
				$month         = date ( "F" );
				$template_name = "Monthly Billing Accounting Warning";
				if ( preg_match ( "/charge/i" , $do ) ) {
						$template_name = "Monthly Billing Accounting Update";
				}
				## Grab Email Template ##
				$template_text = $templates[ 'jackhammer_admin' ][ 'default' ][ 'EMAIL' ][ $template_name ];
				print "Num days in month ($month) is: $num_days\n\n";
				$SQL      = "SELECT acct_id FROM " . ACCOUNTS_TABLE;
				$accounts = $iSQL->search ( array ( "SQL" => $SQL , "Return" => 'array' ) );
				foreach ( $accounts as $id ) {
						$user         = new Account( array ( "Parent" => $iSQL , "Lookup" => 1 , "Table" => ACCOUNTS_TABLE , "Schema" => ACCOUNTS_TABLE , "acct_id" => $id ) );
						$links        = $user->get_links ();
						$amount_due   = 0.00;
						$keywordlist  = array ();
						$catlist      = array ();
						$user_summary = "
             <b>$id</b><br />
             <table width='80%' align='center' bgcolor='navy' border='0'>
               <tr bgcolor='blue'>
                 <th><font color='white'>$language[manage_bids_keyword]</font></th>
                 <th><font color='white'>$language[manage_bids_amount]</font></th>
               </tr>
          ";
						foreach ( $links as $website ) {
								$user_summary .= "
                 <tr bgcolor='#e0e0e0'>
                   <td colspan='2'>
                      <b>
                      $website[title]<br />
                      $website[url]
                      </b>
                   </td>
                 </tr>
              ";
								$link_due   = 0.00;
								$link       = new Listing( array ( "Parent" => $iSQL , "Lookup" => 1 , "Table" => LINKS_TABLE , "Schema" => LINKS_TABLE , "id" => $website[ id ] ) );
								$keys       = $link->fetch_bids ();
								$categories = $link->fetch_categories ();
								$original   = array ();
								$bids       = array ();
								foreach ( $keys as $keyword => $price ) {
										$user_summary .= "
                    <tr bgcolor='#e0e0e0'>
                      <td>$keyword</td>
                      <td>$language[dollar_sign] $price</td>
                    </tr>
                  ";
										$amount_due += $price;
										$link_due += $price;
								}
								foreach ( $categories as $category ) {
										$catname  = $category->data[ category ];
										$catprice = $category->data[ bid_price ];
										$user_summary .= "
                    <tr bgcolor='#e0e0e0'>
                      <td>Category: $catname</td>
                      <td>$language[dollar_sign] $catprice</td>
                    </tr>
                  ";
										$amount_due += $catprice;
										$link_due += catprice;
								}
								$user_summary .= "
                 <tr bgcolor='#e0e0e0'>
                   <td colspan='2' align='right'>
                     $language[report_subtotal_header] $language[dollar_sign] $link_due
                   </td>
                 </tr>
              ";
						}
						$user_summary .= "
             </table>
          ";
						$prev_balance = $user->data[ current_balance ];
						$new_balance  = $prev_balance - $amount_due;
						## Prepare the Email ##
						$emailinfo = array ( "body" => $user_summary , "month" => $month , "amount_due" => $amount_due , "previous_balance" => $prev_balance , "current_balance" => $new_balance , );
						$template  = new SubTemplate( array ( "front" => "<<" , "back" => ">>" , "template_text" => $template_text , "values" => $emailinfo ) );
						$body      = $template->merge ();
						$to        = $user->data[ contact_email ];
						## Do any accounting work.
						if ( preg_match ( "/charge/i" , $do ) ) {
								## Adjust their balance, make an entry in the table.
								if ( $amount_due > 0.00 && $new_balance >= 0.00 ) {
										$finances                       = new Record( array ( "Parent" => $iSQL , "Table" => TRANSACTIONS_TABLE , "Schema" => TRANSACTIONS_TABLE ) );
										$finances->data[ acct_id ]      = $id;
										$finances->data[ source ]       = $language[ monthly_balance_adjustment ];
										$finances->data[ trans_amount ] = "-$amount_due";
										$finances->data[ trans_date ]   = time;
										$finances->data[ trans_type ]   = "MONTHLY";
										$finances->data[ tax_amount ]   = 0.00;
										$finances->data[ affiliate ]    = $user->data[ field5 ];
										$finances->data[ extra_info ]   = $body;
										$finances->update_values ( $finances->data );
										$finances->save ();
										$user->data[ current_balance ] = $new_balance;
										$user->update_values ( $user->data );
										$user->save ();
								} ## Nuke their Bids ##
								else {
										echo "$id: Negating Bids .... $new_balance would be lower than 0.00\n";
										foreach ( $links as $website ) {
												$id  = $website[ id ];
												$SQL = "UPDATE " . LINKS_TABLE . " SET bid_price = bid_price * -1 WHERE id = $id AND bid_price > 0.00";
												$iSQL->run ( $SQL );
												$SQL = "UPDATE " . LINKS_TABLE . " SET bid_price = bid_price * -1 WHERE id = $id AND bid_price > 0.00";
												$iSQL->run ( $SQL );
										}
								}
						}
						## Send it out.
						$headers = "MIME-Version: 1.0\r\n";
						$headers .= "Content-type: text/html; charset=iso-8859-1\r\n";
						$headers .= "From: $config[email]\r\n";
						mail ( $to , $language[ monthly_billing_subject ] , $body , $headers );
						print "$id:  Total Due: $amount_due (Mailed to $to)\n";
				}
				echo $report;
				exit;
		}


		// Account Auto-Billing.
		// Reads settings from accounts database and dings credit cards based on balance or day of month.
		function cron_autobill ()
		{
				global $iSQL;
				global $CGI;
				global $config;
				global $templates;
				global $include_dir;
				global $data_dir;
				global $ERROR_MESSAGE;
				global $argv;
				// Table Vars
				$accounts     = ACCOUNTS_TABLE;
				$autobill     = AUTOBILL_TABLE;
				$autobill_log = AUTOBILL_LOG_TABLE;
				// Only do the dailies if asked for specifically
				if ( $argv[ 4 ] == "-daily" ) {
						$today = date ( "d" );
						$SQL   = "
               SELECT $accounts.current_balance, $autobill.*
               FROM $autobill, $accounts
               WHERE $autobill.autobill_type = 'Specific Date'
               AND $autobill.autobill_dom = $today
               AND $accounts.acct_id = $autobill.acct_id
           ";
				} else {
						$SQL = "
               SELECT $accounts.current_balance, $autobill.*
               FROM $autobill, $accounts
               WHERE $autobill.autobill_type = 'Low Balance'
               AND $accounts.current_balance <= $autobill.autobill_balance
               AND $accounts.acct_id = $autobill.acct_id
           ";
				}
				$autobill_list = $iSQL->search ( array ( "SQL" => $SQL ) );
				// To properly see things if being run from the browser //
				if ( $_SERVER[ SCRIPT_NAME ] ) {
						echo "<pre>\n";
				}
				foreach ( $autobill_list as $autobill_info ) {
						$account          = new Account( array ( "Parent" => $iSQL , "Table" => ACCOUNTS_TABLE , "Schema" => ACCOUNTS_TABLE , "Lookup" => 1 , "acct_id" => $autobill_info[ acct_id ] ) );
						$previous_balance = $account->data[ 'current_balance' ];
						list ( $success , $trans_id , $info ) = do_autobilling ( $autobill_info , $account );
						if ( $success ) {
								// Apply the deposit and reactivate disabled keywords
								$CGI[ amount ] = $autobill_info[ autobill_amount ]; // Needed for the handler
								$info .= "\n" . $account->handle_deposit ( 1 );
								// Log this into their autobilling history
								$autobill_log                         = new Account( array ( "Parent" => $iSQL , "Table" => AUTOBILL_LOG_TABLE , "Schema" => AUTOBILL_LOG_TABLE ) );
								$autobill_log->data[ acct_id ]        = $autobill_info[ acct_id ];
								$autobill_log->data[ amount ]         = $autobill_info[ autobill_amount ];
								$autobill_log->data[ tstamp ]         = time ();
								$autobill_log->data[ transaction_id ] = $trans_id;
								$autobill_log->data[ extra_info ]     = $info;
								$autobill_log->update_values ( $autobill_log->data );
								$autobill_log->save ();
								// Return something visible to the runner //
								$balance = $account->data[ current_balance ];
								echo "
                   Billed: $autobill_info[acct_id]
                   Amount: $autobill_info[autobill_amount]
                   New Balance: $balance
                   Transaction ID: $trans_id
                   ----------------------------------------------------------------
                ";
								if ( $config[ 'jh_autobill_email' ] ) {
										$email_values                       = array ();
										$email_values[ 'acct_id' ]          = $account->data[ 'acct_id' ];
										$email_values[ 'previous_balance' ] = $previous_balance;
										$email_values[ 'current_balance' ]  = $account->data[ 'current_balance' ];
										$email_values[ 'autobill_amount' ]  = $autobill_info[ 'autobill_amount' ];
										$email_values[ 'transaction_id' ]   = $trans_id;
										$ttxt                               = $templates[ 'acct_admin' ][ 'default' ][ 'EMAIL' ][ $config[ 'jh_autobill_email' ] ];
										$template                           = new SubTemplate( array ( "front" => "<<" , "back" => ">>" , "template_text" => $ttxt , "values" => $email_values ) );
										$body                               = $template->merge ();
										mail ( $account->data[ 'contact_email' ] , $config[ 'jh_autobill_email' ] , $body , "From: $config[email]\r\n" );
								}
						} else {
								echo "
                   AUTOBILL FAILURE FOR: $autobill_info[acct_id]
                   Amount: $autobill_info[autobill_amount]
                   $ERROR_MESSAGE
                   $info
                   More info in $data_dir/transaction.log
                   ----------------------------------------------------------------
                ";
						}
				}
		}

		// This function actually dings the credit card //
		function do_autobilling ( $autobill_info , $account_info )
		{
				global $CGI;
				global $config;
				include_once ( "dsX/Addons/gateways/authnet.inc" );
				include_once ( "dsX/Addons/gateways/bofa.inc" );
				include_once ( "dsX/Addons/gateways/linkpoint.inc" );
				include_once ( "dsX/Addons/gateways/verisign.inc" );
				$account   = $account_info->data[ acct_id ];
				$cc_number = decrypt_link ( $account_info->data[ cc_number ] );
				$cc_month  = $account_info->data[ cc_month ];
				$cc_year   = $account_info->data[ cc_year ];
				$cvv       = $account_info->data[ cc_cvv ];
				$amount    = $autobill_info[ autobill_amount ];
				$log       = '';
				$id        = time () + $cvv;
				$success   = 0;
				// Do whatever it takes to bill the account,
				// retrieving any log info in "$log"
				// the transaction id into $id
				// and a '1' into $success if all was well.
				// ---------------------------------------------------------------- //
				$CGI = array ();
				list ( $CGI[ 'first_name' ] , $CGI[ 'last_name' ] ) = preg_split ( "/\s+/" , $account_info->data[ 'cc_name' ] );
				$CGI[ 'name' ]     = $account_info->data[ 'cc_name' ];
				$CGI[ 'company' ]  = $account_info->data[ 'company' ];
				$CGI[ 'address1' ] = $account_info->data[ 'address' ];
				$CGI[ 'address2' ] = $account_info->data[ 'address2' ];
				$CGI[ 'city' ]     = $account_info->data[ 'city' ];
				$CGI[ 'state' ]    = $account_info->data[ 'state' ];
				$CGI[ 'zip' ]      = $account_info->data[ 'zip' ];
				$CGI[ 'country' ]  = $account_info->data[ 'country' ];
				if ( !$CGI[ 'country' ] ) {
						$CGI[ 'country' ] = "US";
				}
				$CGI[ 'phone' ]        = $account_info->data[ 'phone' ];
				$CGI[ 'email' ]        = $account_info->data[ 'contact_email' ];
				$CGI[ 'account' ]      = $account_info->data[ 'acct_id' ];
				$CGI[ 'cc_number' ]    = $cc_number;
				$CGI[ 'cvmvalue' ]     = $cvv;
				$CGI[ 'cardexpmonth' ] = $cc_month;
				$CGI[ 'cardexpyear' ]  = $cc_year;
				$CGI[ 'amount' ]       = $amount;
				$payment_gateway       = $config[ 'payment_gateway' ];
				$gateway               = new $payment_gateway();
				list( $success , $log ) = $gateway->process_payment ();

				// ---------------------------------------------------------------- //
				return array ( $success , $id , $log );
		}

		// Create Merge Tables
		function cron_mergetables ()
		{
				global $iSQL;
				global $CGI;
				global $language;
				global $config;
				global $my_accounts;
				global $SCRIPT_NAME;
				if ( $config[ jackhammer_merge_tables ] != "YES" ) {
						echo "Merge Tables Option Not Turned On\n";
						exit;
				}
				echo "Creating Merge Tables\n";
				// Table Vars
				$clicklog      = CLICK_LOG_TABLE;
				$fraudlog      = CLICK_FRAUD_TABLE;
				$debuglog      = CLICK_DEBUG_TABLE;
				$searchlog     = LOG_TABLE;
				$cl_search     = CLICK_LOG_TABLE . "_2%";
				$cf_search     = CLICK_FRAUD_TABLE . "_2%";
				$cd_search     = CLICK_DEBUG_TABLE . "_2%";
				$hs_search     = LOG_TABLE . "_2%";
				$mrg_jhcl_SQL  = "SHOW TABLES LIKE '$cl_search'";
				$cl_log_tables = $iSQL->search ( array ( "SQL" => $mrg_jhcl_SQL , "Return" => "array" ) );
				$mrg_jhcf_SQL  = "SHOW TABLES LIKE '$cf_search'";
				$fraud_tables  = $iSQL->search ( array ( "SQL" => $mrg_jhcf_SQL , "Return" => "array" ) );
				/*   $mrg_jhdbg_SQL = "SHOW TABLES LIKE '$cd_search'";
							$debug_tables = $iSQL->search( array("SQL"=>$mrg_jhdbg_SQL, "Return"=>"array") );*/
				$mrg_log_SQL = "SHOW TABLES LIKE '$hs_search'";
				$log_tables  = $iSQL->search ( array ( "SQL" => $mrg_log_SQL , "Return" => "array" ) );
				$cl          = implode ( "," , $cl_log_tables );
				$cf          = implode ( "," , $fraud_tables );
				$dbg         = implode ( "," , $debug_tables );
				$logs        = implode ( "," , $log_tables );
				$SQL         = "DROP TABLE IF EXISTS " . CLICK_LOG_TABLE;
				$iSQL->run ( $SQL );
				$SQL = "DROP TABLE IF EXISTS " . CLICK_FRAUD_TABLE;
				$iSQL->run ( $SQL );
				/* $SQL = "DROP TABLE IF EXISTS " . CLICK_DEBUG_TABLE;
					$iSQL->run ( $SQL );*/
				$SQL = "DROP TABLE IF EXISTS " . LOG_TABLE;
				$iSQL->run ( $SQL );
				$SQL = "
                CREATE TABLE $clicklog (
                 hit_id int(10) unsigned NOT NULL,
                  link_id int(11) NOT NULL default '0',
                  word varchar(225) NOT NULL default '',
                  ppc_engine varchar(40) NOT NULL default 'internal',
                  ppc_commission varchar(10) default NULL,
                  gross_price decimal(5,3) NOT NULL default '0.000',
                  price decimal(5,3) NOT NULL default '0.000',
                  hit_date int(11) NOT NULL default '0',
                  hit_time int(11) NOT NULL default '0',
                  affiliate varchar(50) NOT NULL default '',
                  account varchar(50) NOT NULL default '',
                  commission varchar(10) default NULL,
                  paid_flag char(3) default NULL,
                  roi_code int(11) NOT NULL default '0',
                  roi_sale int(11) default NULL,
                  debug_info varchar(100) default NULL,
                  ip varchar(15) default NULL,
                  referer varchar(200) default NULL,
                  user_agent varchar(200) default NULL,
				   subid varchar(50) default NULL,
				    ad_id int(11) default NULL,
					 group_id int(11) default NULL,
                      country varchar(100) default NULL,
                  PRIMARY KEY (hit_id),
                  KEY jhcl_date(hit_date),
                  KEY jhcl_src(hit_date,ppc_engine),
                  KEY jhcl_id(hit_date,link_id),
                  KEY jhcl_acct(hit_date,account),
                  KEY jhcl_aff(hit_date,affiliate),
                  KEY jhcl_word(hit_date,word),
                  KEY jhcl_camp_id(link_id),
                  KEY jhcl_group_id(group_id),
                  KEY jhcl_ad_id(ad_id)
                )
                ENGINE=MRG_MyISAM DEFAULT CHARSET=latin1 COLLATE=latin1_swedish_ci UNION=(" . $cl . ")
            ";
				$iSQL->run ( $SQL );
				echo "jh_click_log done\n";
				$SQL = "
                CREATE TABLE $fraudlog (
                 CREATE TABLE $click_fraud (
                 hit_id int(10) unsigned NOT NULL,
				 click_tstamp int(11) NOT NULL default '0',
				 fraud_id int(11) NOT NULL default '0',
				  link_id int(11) NOT NULL default '0',
				   word varchar(225) NOT NULL default '',
				    price decimal(5,3) NOT NULL default '0.000',
					hit_date int(11) NOT NULL default '0',
					affiliate varchar(50) NOT NULL default '',
					account varchar(50) NOT NULL default '',
					ppc_engine varchar(40) NOT NULL default 'internal',
					ip_number varchar(20) default NULL,
					user_agent varchar(100) default NULL,
					referer varchar(200) default NULL,
					 debug_info varchar(100) default NULL,
					  fraud_reason varchar(200) default NULL,
					   subid varchar(50) default NULL,
					   `ad_id` int(11) default NULL,
  `group_id` int(11) default NULL, country varchar(100) default NULL, PRIMARY KEY (hit_id), KEY jhfl_hitid(fraud_id), KEY jhfl_date(hit_date), KEY jhfl_src(hit_date,ppc_engine), KEY jhfl_id(hit_date,link_id), KEY jhfl_acct(hit_date,account), KEY jhfl_aff(hit_date,affiliate), KEY jhfl_word(hit_date,word)
                )
                 ENGINE=MRG_MyISAM DEFAULT CHARSET=latin1 COLLATE=latin1_swedish_ci UNION=($cf)
            ";
				$iSQL->run ( $SQL );
				echo "jh_click_fraud done\n";
				$SQL = "
                CREATE TABLE $debuglog (
                  cid varchar(50) NOT NULL default '',
                  click_date int(11) NOT NULL default '0',
                  fraud int(11) NOT NULL default '0',
                  keyword varchar(150) default NULL,
                  price decimal(9,3) default NULL,
                  affiliate varchar(80) NOT NULL default '',
                  ip varchar(15) default NULL,
                  referer varchar(200) default NULL,
                  user_agent varchar(200) default NULL,
                  link_id int(11) default NULL,
                  account varchar(80) default NULL,
                  debug_text text,
                  PRIMARY KEY (cid),
                  KEY jd1(click_date),
                  KEY jd2(click_date,account),
                  KEY jd3(click_date,affiliate),
                  KEY jd4(click_date,link_id)
                )
                TYPE=MERGE UNION=($dbg)
            ";
				//  $iSQL->run ( $SQL );
				echo "jackhammer_debug done\n";
				$SQL = "
                CREATE TABLE $searchlog (

				`logdate` int(11) NOT NULL default '0',
				  `search_type` char(120) NOT NULL default '',
				  `criteria` char(120) NOT NULL default '',
				  `ip_number` char(20) NOT NULL default '',
				  `user_agent` char(100) NOT NULL default '',
				  `referer` char(255) NOT NULL default '',
				  `num_results` int(11) default NULL,
				`affiliate` VARCHAR(255) NULL DEFAULT NULL,
				  `subid` varchar(255) default NULL,
				  `search_time` int(11) NOT NULL default '0',
				  KEY `hslog1` (`logdate`),
				  KEY `hslog2` (`search_type`),
				  KEY `hslog3` (`criteria`),
				  KEY `hslog4` (`ip_number`),
				  KEY `hslog5` (`user_agent`),
				  KEY `hslog6` (`referer`),
				 KEY hslog7 (affiliate),
				  KEY hslog8 (subid)
                )
				ENGINE=MRG_MyISAM DEFAULT CHARSET=latin1 COLLATE=latin1_swedish_ci UNION=($logs)

            ";
				$iSQL->run ( $SQL );
				echo "hyperseek_log done\n";
		}


		// Re-Summarize the clicks and fraud (synchronize the summary tables with the raw click/fraud logs
		function cron_summarize_clicks ()
		{
				global $iSQL;
				global $CGI;
				global $language;
				global $config;
				global $my_accounts;
				global $SCRIPT_NAME;
				global $data_dir;
				global $argv;
				$report         = '';
				$aff_summary    = array ();
				$source_summary = array ();
				$click_summary  = array ();
				$daily_summary  = array ();
				$log_summary    = array ();
				$LOG            = '';
				$start          = time ();
				$date           = $argv[ 4 ];
				if ( $date == "all" ) {
						$SQL = "SELECT distinct hit_date FROM " . CLICK_LOG_TABLE;
				} elseif ( $date ) {
						$SQL = "SELECT distinct hit_date FROM " . CLICK_LOG_TABLE . " WHERE hit_date = $date";
				} else {
						$time       = time - 86400; ## 24 hours ago
						$datestring = date ( "Ymd" );
						$SQL        = "SELECT distinct hit_date FROM " . CLICK_LOG_TABLE . " WHERE hit_date = $datestring";
				}
				$dates = $iSQL->search ( array ( "SQL" => $SQL , "Return" => "array" ) );
				foreach ( $dates as $hit_date ) {
						echo "Working on $hit_date\n";
						$LOG .= "Working on $hit_date\n";
						$SQL    = "SELECT * FROM " . CLICK_LOG_TABLE . " WHERE hit_date = $hit_date";
						$clicks = $iSQL->search ( array ( "SQL" => $SQL ) );
						foreach ( $clicks as $click ) {
								$date            = $click[ hit_date ];
								$source          = $click[ ppc_engine ];
								$affiliate       = $click[ affiliate ];
								$account         = $click[ account ];
								$gross_price     = $click[ gross_price ];
								$price           = $click[ price ];
								$ppc_commission  = $click[ ppc_commisssion ];
								$aff_commission  = $click[ commission ];
								$paid_status     = $click[ paid_status ];
								$keyword         = $click[ word ];
								$id              = $click[ link_id ];
								$ppc_calc_factor = $ppc_commission;
								if ( preg_match ( '/%/' , $ppc_commission ) ) {
										$ppc_calc_factor = $ppc_commission;
										$ppc_calc_factor = preg_replace ( "/\%/" , "" , $ppc_calc_factor );
										$ppc_calc_factor = $ppc_calc_factor * .01;
								}
								$aff_calc_factor = $aff_commission;
								if ( preg_match ( '/%/' , $aff_commission ) ) {
										$aff_calc_factor = $aff_commission;
										$aff_calc_factor = preg_replace ( "/\%/" , "" , $aff_calc_factor );
										$aff_calc_factor = $aff_calc_factor * .01;
								}
								$aff_earnings = $price * $aff_calc_factor;
								$ppc_earnings = $price * $ppc_calc_factor;
								## Build up the hashes ##
								$aff_summary[ $date ][ $affiliate ][ acct_id ]      = $affiliate;
								$aff_summary[ $date ][ $affiliate ][ summary_date ] = $date;
								$aff_summary[ $date ][ $affiliate ][ click_count ]++;
								$aff_summary[ $date ][ $affiliate ][ click_amount_net ] += $aff_earnings;
								$aff_summary[ $date ][ $affiliate ][ click_amount_gross ] += $price;
								$source_summary[ $date ][ $source ][ source ]       = $source;
								$source_summary[ $date ][ $source ][ summary_date ] = $date;
								$source_summary[ $date ][ $source ][ click_count ]++;
								$source_summary[ $date ][ $source ][ click_amount_net ] += $ppc_earnings;
								$source_summary[ $date ][ $source ][ click_amount_gross ] += $price;
								$click_summary[ $date ][ $account ][ acct_id ]    = $account;
								$click_summary[ $date ][ $account ][ link_id ]    = $id;
								$click_summary[ $date ][ $account ][ click_date ] = $date;
								$click_summary[ $date ][ $account ][ click_count ]++;
								$click_summary[ $date ][ $account ][ click_amount_gross ] += $price;
								$click_summary[ $date ][ $account ][ click_amount_net ] += $aff_earnings;
								$daily_summary[ $date ][ summary_date ] = $date;
								$daily_summary[ $date ][ click_count ]++;
								$daily_summary[ $date ][ click_amount_gross ] += $price;
								$daily_summary[ $date ][ click_amount_net ] += $aff_earnings;
								$keyword                                      = preg_replace ( "/\'/" , "" , $keyword );
								$keyword                                      = preg_replace ( "/\\/" , "" , $keyword );
								$log_summary[ $date ][ $keyword ][ criteria ] = $keyword;
								$log_summary[ $date ][ $keyword ][ click_count ]++;
								$log_summary[ $date ][ $keyword ][ click_amount_gross ] += $price;
								$log_summary[ $date ][ $keyword ][ click_amount_net ] += $aff_earnings;
						}
				}
				## Now, go through the lot, and save them ##
				## Affiliates
				echo "Affiliates...";
				$LOG .= "Affiliates...\n";
				foreach ( $aff_summary as $date ) {
						foreach ( $aff_summary[ date ] as $affrecord ) {
								$data                       = $aff_summary[ $date ][ $affrecord ];
								$data[ click_amount_net ]   = sprintf ( "%5.2f" , $data[ click_amount_net ] );
								$data[ click_amount_gross ] = sprintf ( "%5.2f" , $data[ click_amount_gross ] );
								$record                     = new Record( array ( "Parent" => $iSQL , "Table" => AFF_SUMMARY_TABLE , "Schema" => AFF_SUMMARY_TABLE , "Lookup" => 1 , "acct_id" => $data[ acct_id ] , "summary_date" => $data[ summary_date ] ) );
								$record->update_values ( $data );
								$record->save ();
								$LOG .= "$data[summary_date]: $data[acct_id] ... $data[click_count] / $data[click_amount_net] / $data[click_amount_gross]\n";
						}
				}
				echo "Done\n";
				$LOG .= "\n\n";
				## Accounts
				echo "Accounts...";
				$LOG .= "Accounts...\n";
				foreach ( $click_summary as $date ) {
						foreach ( $click_summary[ $date ] as $acctrecord ) {
								$data                       = $click_summary[ $date ][ $acctrecord ];
								$data[ click_amount_net ]   = sprintf ( "%5.2f" , $data[ click_amount_net ] );
								$data[ click_amount_gross ] = sprintf ( "%5.2f" , $data[ click_amount_gross ] );
								$record                     = new Record( array ( "Parent" => $iSQL , "Table" => CLICK_SUMMARY_TABLE , "Schema" => CLICK_SUMMARY_TABLE , "Lookup" => 1 , "acct_id" => $data[ acct_id ] , "link_id" => $data[ link_id ] , "click_date" => $data[ click_date ] ) );
								$record->update_values ( $data );
								$record->save ();
								$LOG .= "$data[click_date]: $data[link_id]:  $data[acct_id] ... $data[click_count] / $data[click_amount_net] / $data[click_amount_gross]\n";
						}
				}
				echo "Done\n";
				$LOG .= "\n\n";
				## Engines
				echo "Engines...";
				$LOG .= "Engines...\n";
				foreach ( $source_summary as $date ) {
						foreach ( $source_summary[ $date ] as $srcrecord ) {
								$data                       = $source_summary[ $date ][ $srcrecord ];
								$data[ click_amount_net ]   = sprintf ( "%5.2f" , $data[ click_amount_net ] );
								$data[ click_amount_gross ] = sprintf ( "%5.2f" , $data[ click_amount_gross ] );
								$record                     = new Record( array ( "Parent" => $iSQL , "Table" => SOURCE_SUMMARY_TABLE , "Schema" => SOURCE_SUMMARY_TABLE , "Lookup" => 1 , "source" => $data[ source ] , "summary_date" => $data[ summary_date ] ) );
								$record->update_values ( $data );
								$record->save ();
								$LOG .= "$data[summary_date]: $data[source] ... $data[click_count] / $data[click_amount_net] / $data[click_amount_gross]\n";
						}
				}
				echo "Done\n";
				$LOG .= "\n\n";
				## Daily
				echo "Daily...";
				$LOG .= "Daily...\n";
				foreach ( $daily_summary as $dailyrecord ) {
						$data                       = $daily_summary[ $dailyrecord ];
						$data[ click_amount_net ]   = sprintf ( "%5.2f" , $data[ click_amount_net ] );
						$data[ click_amount_gross ] = sprintf ( "%5.2f" , $data[ click_amount_gross ] );
						$record                     = new Record( array ( "Parent" => $iSQL , "Table" => DAILY_SUMMARY_TABLE , "Schema" => DAILY_SUMMARY_TABLE , "Lookup" => 1 , "summary_date" => $data[ summary_date ] ) );
						$record->update_values ( $data );
						$record->save ();
						$LOG .= "$data[summary_date]: $data[click_count] / $data[click_amount_net] / $data[click_amount_gross]\n";
				}
				echo "Done\n";
				$LOG .= "\n\n";
				## Log
				echo "Search Log...";
				$LOG .= "Search Log...\n";
				foreach ( $log_summary as $date ) {
						foreach ( $log_summary[ $date ] as $logrecord ) {
								$data                       = $log_summary[ $date ][ $logrecord ];
								$data[ click_amount_net ]   = sprintf ( "%5.2f" , $data[ click_amount_net ] );
								$data[ click_amount_gross ] = sprintf ( "%5.2f" , $data[ click_amount_gross ] );
								$record                     = new Record( array ( "Parent" => $iSQL , "Table" => LOG_SUMMARY_TABLE , "Schema" => LOG_SUMMARY_TABLE , "Lookup" => 1 , "criteria" => $data[ criteria ] ) );
								$record->update_values ( $data );
								$record->save ();
								$LOG .= "$data[criteria] ... $data[click_count] / $data[click_amount_net] / $data[click_amount_gross]\n";
						}
				}
				echo "Done\n";
				$end     = time ();
				$elapsed = sprintf ( "%5.2f" , ( $end - $start ) / 60 );
				echo "\n\nComplete: $elapsed minutes processing time\n\n";
				$LOG .= "\n\nComplete: $elapsed minutes processing time\n\n";
				write_file ( "$data_dir/summary.log" , $LOG );
				exit;
		}

		// Table Cleanup for Jackhammer Fraud Checks
		function cron_table_cleanup ()
		{
				global $iSQL;
				global $CGI;
				global $language;
				global $config;
				global $my_accounts;
				global $SCRIPT_NAME;
				echo "Cleaning Up Tables\n";
				$report = '';
				// Table Vars
				$access  = AFF_ACCESS_TABLE;
				$clicked = JH_CLICKED_TABLE;
				### Nuke the table...
				$SQL = "DROP TABLE " . AFF_ACCESS_TABLE;
				if ( $iSQL->run ( $SQL ) ) {
						$report .= "$language[cron_affiliate_access_table] $language[cron_dropped]\n";
				} else {
						$report .= "$language[cron_affiliate_access_table] $language[cron_not_dropped]\n";
				}
				### Nuke the table...
				$SQL = "DROP TABLE " . JH_CLICKED_TABLE;
				if ( $iSQL->run ( $SQL ) ) {
						$report .= "$language[cron_affiliate_access_table] $language[cron_dropped]\n";
				} else {
						$report .= "$language[cron_affiliate_access_table] $language[cron_not_dropped]\n";
				}
				### Rebuild the tables...
				$SQL = "
            CREATE TABLE $access (
              affiliate char(80),
              ip_number char(30) DEFAULT '' NOT NULL,
              link_id int(11) DEFAULT '0' NOT NULL,
              link_id_count int(11),
              search_terms char(224) DEFAULT '' NOT NULL,
              num_searches int(11),
              KEY hsl_access1 (ip_number,link_id),
              KEY hsl_access2 (ip_number,search_terms)
            )
       ";
				if ( $iSQL->run ( $SQL ) ) {
						$report .= "affiliate_access $language[cron_created]\n";
				} else {
						$report .= "affiliate_access $language[cron_not_created]\n";
				}
				$SQL = "
            CREATE TABLE $clicked (
               id VARCHAR(100) NOT NULL PRIMARY KEY
            )
       ";
				if ( $iSQL->run ( $SQL ) ) {
						$report .= "jh_clicked $language[cron_created]\n";
				} else {
						$report .= "jh_clicked $language[cron_not_created]\n";
				}
				echo $report;
				exit;
		}

		function cron_generate_related_keywords ()
		{
				global $iSQL;
				global $language;
				echo "<pre>\n";
				$SQL      = "SELECT id, keyword FROM " . KEYS_TABLE . " WHERE bid_price > 0.00 ORDER BY id, keyword";
				$keywords = $iSQL->search ( array ( "SQL" => $SQL ) );
				$terms    = array ();
				$listings = array ();
				$related  = array ();
				// Setup arrays of id=>keywords and keywords=>ids to build up the related list
				foreach ( $keywords as $bid ) {
						$term = $bid[ 'keyword' ];
						$id   = $bid[ 'id' ];
						if ( preg_match ( "/" . $term . "/i" , $language[ 'hot_zone_default_term' ] ) ) {
								continue;
						}
						if ( !is_array ( $terms[ $term ] ) ) {
								$terms[ $term ] = array ();
						}
						array_push ( $terms[ $term ] , $id );
						if ( !is_array ( $listings[ $id ] ) ) {
								$listings[ $id ] = array ();
						}
						array_push ( $listings[ $id ] , $term );
				}
				// Go through the keywords
				$rid = 1;
				foreach ( $terms as $term => $ids ) {
						// Assign all keywords from all ids assigned to this keyword the same ID ($rid)
						foreach ( $ids as $id ) {
								foreach ( $listings[ $id ] as $word ) {
										if ( !$related[ $word ] ) {
												$rid++;
										}
										$related[ $word ][ 'id' ] = $rid;
										$related[ $word ][ 'related' ][ $term ]++;
								}
						}
				}
				// ReBuild the table
				$SQL = "DROP TABLE IF EXISTS " . RELATED_KEYS_TABLE;
				$iSQL->run ( $SQL );
				$rkt = RELATED_KEYS_TABLE;
				$SQL = "
           CREATE TABLE $rkt(
              related_id INT NOT NULL,
              keyword VARCHAR(80) NOT NULL,
              num_bids INT,
              num_searches INT,
              num_clicks INT,
              top_bid DECIMAL(5,2),
              key rkid(related_id),
              key rkword(keyword)
           )
       ";
				$iSQL->run ( $SQL );
				$completed = 0;
				foreach ( $related as $word => $info ) {
						foreach ( $info[ 'related' ] as $term => $count ) {
								if ( $count < 2 ) {
										continue;
								}
								$lword               = preg_replace ( "/\'/" , "''" , $term );
								$SQL                 = "SELECT * FROM " . LOG_SUMMARY_TABLE . " WHERE criteria = '$lword'";
								$summary             = $iSQL->search ( array ( "SQL" => $SQL , "Return" => "hash" ) );
								$SQL                 = "SELECT max(bid_price) FROM " . KEYS_TABLE . " WHERE keyword = '$lword'";
								$max                 = $iSQL->search ( array ( "SQL" => $SQL , "Return" => "single" ) );
								$r                   = array ();
								$r[ 'related_id' ]   = $info[ 'id' ];
								$r[ 'keyword' ]      = $term;
								$r[ 'num_bids' ]     = $count;
								$r[ 'num_searches' ] = $summary[ 'num_searches' ];
								$r[ 'num_clicks' ]   = $summary[ 'click_count' ];
								$r[ 'top_bid' ]      = $max;
								$record              = new Record( array ( "Parent" => $iSQL , "Table" => RELATED_KEYS_TABLE , "Schema" => RELATED_KEYS_TABLE , "Lookup" => 1 , "related_id" => $r[ 'related_id' ] , "keyword" => $r[ 'keyword' ] ) );
								$record->update_values ( $r );
								$record->save ();
								echo ".";
								$completed++;
								if ( $completed % 100 == 0 ) {
										echo " ($completed)\n";
								}
						}
				}
				echo "\n\n$completed Related Keywords Mapped\n\n";
				echo "</pre>\n";
		}

		// Import Links //


		function cron_nuke ()
		{
				global $iSQL;
				global $iSQL;
				$tables = array ( LINKS_TABLE , KEYS_TABLE , CATS_TABLE );
				foreach ( $tables as $table ) {
						$SQL = "DELETE FROM $table";
						$iSQL->run ( $SQL );
				};
				echo "Links Tables (Links, Keys, Cat Index) Nuked\n";
		}

		function cron_nuke_stats ()
		{
				global $iSQL;
				$tables = array ( CLICK_SUMMARY_TABLE , SOURCE_SUMMARY_TABLE , DAILY_SUMMARY_TABLE , CLICK_LOG_TABLE , CLICK_FRAUD_TABLE , CLICK_DEBUG_TABLE , JH_CLICKED_TABLE , AFF_PAYMENTS_TABLE , AFF_SEARCHES_TABLE , AFF_ACCESS_TABLE , AFF_SUMMARY_TABLE , LOG_TABLE , LOG_SUMMARY_TABLE );
				foreach ( $tables as $table ) {
						$SQL = "DELETE FROM $table";
						$iSQL->run ( $SQL );
				};
				echo "Stats Nuked\n";
		}


		function cron_import_cats ()
		{
				global $config;
				global $file;
				global $iSQL;
				global $CGI;
				global $argv;
				$file = $argv[ 4 ];
				echo "CONVERTING CATEGORIES\n";
				echo "---------------------\n";
				// echo "Deleting Existing Cats ... ";
				// $SQL = "DELETE FROM " . CATEGORY_TABLE;
				// $iSQL->run($SQL);
				// echo "\n";
				$cats = file ( $file );
				## Determine the max ID
				$SQL   = "SELECT max(category_id) FROM " . CATEGORY_TABLE;
				$max   = $iSQL->search ( array ( "SQL" => $SQL , "Return" => "single" ) );
				$saved = array ();
				foreach ( $cats as $category ) {
						$category = preg_replace ( "/^\s+|\s+$|\r|\n|\cM/" , "" , $category );
						if ( defined ( $saved[ $category ] ) ) {
								next;
						}
						$max++;
						$contents = split ( ":" , $category );
						$numc     = count ( $contents ) - 1;
						$qm       = quotemeta ( $contents[ $numc ] );
						if ( $numc >= 1 ) {
								$parent = preg_replace ( "/\:$qm$/" , "" , $category );
						} else {
								$parent = '';
						}
						$CGI              = array ();
						$CGI[ parent_id ] = $saved[ $parent ];
						if ( !$CGI[ parent_id ] ) {
								$CGI[ parent_id ] = '0';
						}
						$CGI[ category_id ]   = $max;
						$CGI[ category_name ] = $contents[ $numc ];
						$CGI[ full_name ]     = $category;
						$CGI[ teaser ]        = 0;
						$CGI[ cat_count ]     = 0;
						$CGI[ last_activity ] = time ();
						$CGI[ last_listing ]  = 1;
						$record               = new Record( array ( "Parent" => $iSQL , "Table" => CATEGORY_TABLE , "Schema" => CATEGORY_TABLE , "Lookup" => 1 , "category_id" => $CGI[ category_id ] ) );
						if ( $record->update_values ( $CGI ) ) {
								if ( $record->save () ) {
										echo "  --- Imported CID: $CGI[category_id] / PID: $CGI[parent_id] -> $category ($CGI[category_name])\n";
								} else {
										echo "Error: $record->error_text\n";
								}
						} else {
								echo "Error: ($CGI[parent_id]) ... $record->error_text\n";
						}
						$saved[ $category ] = $max;
				}
				echo "\n\n CATEGORY CONVERSION DONE \n";
		}


		function cron_import_links ()
		{
				global $CGI;
				global $file;
				global $iSQL;
				global $argv;
				$file      = $argv[ 4 ];
				$hyperseek = new hyperseekAdmin();
				echo "Loading Data ... From $file\n";
				## Category ID Hash ##
				echo "Generating Category Tree ...";
				$SQL  = "SELECT full_name, category_id FROM " . CATEGORY_TABLE;
				$cats = $iSQL->search ( array ( "SQL" => $SQL , "Return" => "fullhash" ) );
				echo "Got it.\n";
				$genid   = time ();
				$count   = 0;
				$entries = file ( $file );
				foreach ( $entries as $line ) {
						$line = preg_replace ( "/^\s+|\s+$|\r|\n|\cM/" , "" , $line );
						if ( !$line ) {
								continue;
						}
						$CGI = array ();
						list ( $CGI[ 'id' ] , $CGI[ 'category' ] , $CGI[ 'extra_link' ] , $CGI[ 'rank' ] , $CGI[ 'qualifier' ] , $CGI[ 'email' ] , $CGI[ 'title' ] , $CGI[ 'url' ] , $CGI[ 'description' ] , $CGI[ 'keywords' ] , $CGI[ 'desc_image' ] , $CGI[ 'title_tag' ] , $CGI[ 'site_icon' ] , $CGI[ 'name' ] , $CGI[ 'address' ] , $CGI[ 'city' ] , $CGI[ 'state' ] , $CGI[ 'zip' ] , $CGI[ 'country' ] , $CGI[ 'phone' ] , $CGI[ 'fax' ] , $CGI[ 'startdate' ] , $CGI[ 'enddate' ] , $CGI[ 'price' ] , $CGI[ 'account' ] , $CGI[ 'xfactor' ] , $CGI[ 'field1' ] , $CGI[ 'field2' ] , $CGI[ 'field3' ] , $CGI[ 'field4' ] , $CGI[ 'field5' ] , $CGI[ 'field6' ] , $CGI[ 'field7' ] , $CGI[ 'field8' ] , $CGI[ 'field9' ] , $CGI[ 'field10' ] ) = preg_split ( '/\|/' , $line );
						## Defaults
						if ( !$CGI[ 'id' ] ) {
								$CGI[ 'id' ] = $genid;
						}
						$genid++;
						$assign_cat = '';
						foreach ( split ( ';' , $CGI[ 'category' ] ) as $cat ) {
								$this_category_id = $cats[ $cat ];
								if ( is_numeric ( $cat ) ) {
										$this_category_id = $cat;
								} else if ( !$this_category_id ) {
										$this_category_id = -1;
								}
								if ( $assign_cat ) {
										$assign_cat .= ";";
								}
								$assign_cat .= $this_category_id;
						}
						$CGI[ 'category' ] = $assign_cat;
						if ( !( $CGI[ 'rank' ] ) ) {
								$CGI[ 'rank' ] = 1;
						}
						if ( !( $CGI[ 'email' ] ) ) {
								$CGI[ 'email' ] = "x";
						}
						$output = $hyperseek->save_link ( 1 );
						$count++;
						echo "$count Loaded   -> $output\n";
				}
				echo "Link Information Loaded\n\n";
		}


		function cron_import_into_queue ()
		{
				global $CGI;
				global $file;
				global $iSQL;
				global $argv;
				$file = $argv[ 4 ];
				echo "Loading Data ... From $file\n";
				## Category ID Hash ##
				echo "Generating Category Tree ...";
				$SQL  = "SELECT full_name, category_id FROM " . CATEGORY_TABLE;
				$cats = $iSQL->search ( array ( "SQL" => $SQL , "Return" => "fullhash" ) );
				echo "Got it.\n";
				$genid   = time ();
				$count   = 0;
				$entries = file ( $file );
				foreach ( $entries as $line ) {
						$line = preg_replace ( "/^\s+|\s+$|\r|\n|\cM/" , "" , $line );
						if ( !$line ) {
								continue;
						}
						$CGI = array ();
						list ( $CGI[ 'id' ] , $CGI[ 'category' ] , $CGI[ 'extra_link' ] , $CGI[ 'rank' ] , $CGI[ 'qualifier' ] , $CGI[ 'email' ] , $CGI[ 'title' ] , $CGI[ 'url' ] , $CGI[ 'description' ] , $CGI[ 'keywords' ] , $CGI[ 'desc_image' ] , $CGI[ 'title_tag' ] , $CGI[ 'site_icon' ] , $CGI[ 'name' ] , $CGI[ 'address' ] , $CGI[ 'city' ] , $CGI[ 'state' ] , $CGI[ 'zip' ] , $CGI[ 'country' ] , $CGI[ 'phone' ] , $CGI[ 'fax' ] , $CGI[ 'startdate' ] , $CGI[ 'enddate' ] , $CGI[ 'price' ] , $CGI[ 'account' ] , $CGI[ 'xfactor' ] , $CGI[ 'field1' ] , $CGI[ 'field2' ] , $CGI[ 'field3' ] , $CGI[ 'field4' ] , $CGI[ 'field5' ] , $CGI[ 'field6' ] , $CGI[ 'field7' ] , $CGI[ 'field8' ] , $CGI[ 'field9' ] , $CGI[ 'field10' ] ) = preg_split ( '/\|/' , $line );
						## Defaults
						if ( !$CGI[ 'id' ] ) {
								$CGI[ 'id' ] = $genid;
						}
						$genid++;
						$assign_cat = '';
						foreach ( split ( ';' , $CGI[ 'category' ] ) as $cat ) {
								$this_category_id = $cats[ $cat ];
								if ( !$this_category_id ) {
										$this_category_id = -1;
								}
								if ( $assign_cat ) {
										$assign_cat .= ";";
								}
								$assign_cat .= $this_category_id;
						}
						$CGI[ 'category' ] = $assign_cat;
						if ( !( $CGI[ 'rank' ] ) ) {
								$CGI[ 'rank' ] = 1;
						}
						if ( !( $CGI[ 'email' ] ) ) {
								$CGI[ 'email' ] = "x";
						}
						$CGI[ 'submission_type' ] = "SPIDERED";
						$CGI[ 'submitted_by' ]    = "SPIDER";
						$link                     = new Record( array ( "Parent" => $iSQL , "Table" => TEMP_TABLE , "Schema" => TEMP_TABLE ) );
						$link->update_values ( $CGI );
						$link->save ();
						$count++;
						echo "$count Loaded   -> $output\n";
				}
				echo "Queue Information Loaded\n\n";
		}


		// Generate New Keyword Minimums
		function cron_keyword_minimums ()
		{
				echo calculate_keyword_minimums ( 1 );
				exit;
		}


		// Check & Repair accounts and links with maximum daily limits

		function cron_daily_limits ()
		{
				global $iSQL;
				$today        = date ( "Ymd" );
				$begin_week   = date ( "Ymd" , mktime ( 0 , 0 , 0 , date ( "m" ) , date ( "d" ) + ( -1 * ( date ( "w" ) ) ) , date ( "Y" ) ) );
				$begin_month  = date ( "Ymd" , mktime ( 0 , 0 , 0 , date ( "m" ) , 1 , date ( "Y" ) ) );
				$SQL          = "SELECT acct_id FROM " . ACCOUNTS_TABLE . " WHERE max_period IN('Day','Week','Month')";
				$all_accounts = $iSQL->search ( array ( "SQL" => $SQL , "Return" => "array" ) );
				foreach ( $all_accounts as $account_holder ) {
						// This creates an "account object" from the account ID that we have in our loop.
						$account        = new Account( array ( "Parent" => $iSQL , "Table" => ACCOUNTS_TABLE , "Schema" => ACCOUNTS_TABLE , "Lookup" => 1 , "acct_id" => $account_holder ) );
						$account_max    = $account->data[ 'max_limit' ];
						$account_period = $account->data[ 'max_period' ];
						echo "Account: $account_holder ... ";
						if ( $account_max >= 0.00 && preg_match ( "/day|week|month/i" , $account_period ) ) {
								if ( $account_period == 'Day' ) {
										echo "Daily Limt for this account ... re-enabling all links";
										$account->enable_all_links ();
								} else if ( $account_period == 'Week' ) {
										if ( $today == $begin_week ) {
												echo "$today is the beginning of the week so all account links are enabled...";
												$account->enable_all_links ();
										}
								} else if ( $account_period == 'Month' ) {
										if ( $today == $begin_month ) {
												echo "$today is the beginning of the week so all account links are enabled ... ";
												$account->enable_all_links ();
										}
								}
						}
						echo "Done\n";
				} // End Foreach Accounts //
				$SQL       = "SELECT id FROM " . LINKS_TABLE . " WHERE max_period IN('Day','Week','Month')";
				$all_links = $iSQL->search ( array ( "SQL" => $SQL , "Return" => "array" ) );
				foreach ( $all_links as $link_id ) {
						$link        = new Listing( array ( "Parent" => $iSQL , "Table" => LINKS_TABLE , "Schema" => LINKS_TABLE , "Lookup" => 1 , "id" => $link_id ) );
						$link_max    = $link->data[ 'max_limit' ];
						$link_period = $link->data[ 'max_period' ];
						echo "Link: $link_id ... ";
						if ( $link_max >= 0.00 && preg_match ( "/day|week|month/i" , $link_period ) ) {
								if ( $link_period == 'Day' ) {
										echo "Daily Limt for this link ... re-enabling ... ";
										$link->turn_on ();
								} else if ( $link_period == 'Week' ) {
										if ( $today == $begin_week ) {
												echo "$today is the beginning of the week so link is enabled... ";
												$link->turn_on ();
										}
								} else if ( $link_period == 'Month' ) {
										if ( $today == $begin_month ) {
												echo "$today is the beginning of the week so link is enabled ... ";
												$link->turn_on ();
										}
								}
						}
						echo "Done\n";
				}
		}


?>
