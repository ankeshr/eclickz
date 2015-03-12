<?php

		/*

		________________________________________________________________________________

	     		PROGRAM : ASP : Admin : Core Admin Center
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

 
		function affiliate_report ()
		{
                    
				global $language;
				$CGI[ acct_id ] = $CGI[ affiliate ];
				$report_form    = jackhammer_report_form ( "Affiliates" );
				admin_output ( "$language[aff_report_title] $CGI[affiliate]" , $report_form );
		}


		// ------------------------------------------------------------------------------- //
		// REPORTS
		// ------------------------------------------------------------------------------- //
		function affiliate_payment_hist ()
		{
                    
				global $oSQL;
				global $CGI;
				global $language;
				$account = new Account( array ( "Parent" => $oSQL , "Table" => ACCOUNTS_TABLE , "Schema" => ACCOUNTS_TABLE , "Lookup" => 1 , "acct_id" => $CGI[ affiliate ] ) );
				$report  = $account->affiliate_payment_history ();
				admin_output ( "$language[aff_history_title] $CGI[affiliate]" , $report );
		}


		function affiliate_performance_report ()
		{
                   
				global $SCRIPT_NAME;
				global $language;
				global $oSQL;
				global $CGI;
				global $config;
				$final_report;
				$grand_total;
				$accounts = '';
				if ( !$CGI[ paid_flag ] ) {
						$CGI[ paid_flag ] = "unpaid";
				}
				if ( $CGI[ acct_id ] ) {
						$accounts = $CGI[ acct_id ];
				} else {
						$SQL        = "SELECT affiliate FROM " . AFF_INFO_TABLE;
						$affiliates = $oSQL->search ( array ( "SQL" => $SQL , "Return" => "array" ) );
						$accounts   = implode ( "','" , $affiliates );
				}
				$end_day          = $CGI[ end_day ];
				$end_month        = $CGI[ end_month ];
				$end_year         = $CGI[ end_year ];
				$start_day        = $CGI[ start_day ];
				$start_month      = $CGI[ start_month ];
				$start_year       = $CGI[ start_year ];
				$starttime        = mktime ( 0 , 0 , 0 , $start_month , $start_day , $start_year );
				$start_string     = "$start_month/$start_day/$start_year";
				$endtime          = mktime ( 23 , 59 , 59 , $end_month , $end_day , $end_year );
				$end_string       = "$end_month/$end_day/$end_year";
				$start_search     = $start_year . $start_month . $start_day;
				$end_search       = $end_year . $end_month . $end_day;
				$rpt_search_total = 0;
				$rpt_click_total  = 0;
				$rpt_rev_total    = 0;
				$report_lines     = array ();
				// Table Name Vars
				$transtable = TRANSACTIONS_TABLE;
				$affsummary = AFF_SUMMARY_TABLE;
				## REVSHARE SUMMARY ##
				$rev_commcheck = " AND commission_paid_date >= 1 ";
				if ( preg_match ( "/unpaid/i" , $CGI[ paid_flag ] ) ) {
						$rev_commcheck = "AND commission_paid_date < 1";
				}
				$SQL      = "
                 SELECT * FROM $transtable
                 WHERE affiliate IN ('$accounts')
                 AND trans_date BETWEEN '$starttime' AND '$endtime' $rev_commcheck
            ";
				$results  = $oSQL->search ( array ( "SQL" => $SQL ) );
				$revshare = array ();
				foreach ( $results as $record ) {
						$affid    = $record[ affiliate ];
						$raw_date = date ( "Ymd" , $record[ trans_date ] );
						$revshare[ $raw_date ][ $affid ][ count ]++;
						$report_lines[ $raw_date ][ $affid ][ revshare ] += $record[ commission_amount ];
				}
				## SEARCH SUMMARY ##
				$search_commcheck = " AND paid_flag = 'YES' ";
				if ( preg_match ( "/unpaid/i" , $CGI[ paid_flag ] ) ) {
						$search_commcheck = " AND paid_flag != 'YES' ";
				}
				$SQL     = "
                 SELECT summary_date, acct_id, sum(search_count) as scount, sum(search_amount) as commission FROM $affsummary
                   WHERE acct_id IN ('$accounts')
                   AND summary_date BETWEEN '$start_search' AND '$end_search'
                   $search_commcheck
                   GROUP BY summary_date, acct_id
            ";
				$results = $oSQL->search ( array ( "SQL" => $SQL ) );
				foreach ( $results as $record ) {
						$date                                            = formatdate ( $record[ summary_date ] );
						$raw_date                                        = $record[ summary_date ];
						$affid                                           = $record[ acct_id ];
						$report_lines[ $raw_date ][ $affid ][ searches ] = $record[ commission ];
				}
				## CLICK SUMMARY ##
				$click_commcheck = " AND paid_flag = 'YES' ";
				if ( preg_match ( "/unpaid/i" , $CGI[ paid_flag ] ) ) {
						$click_commcheck = " AND paid_flag != 'YES' ";
				}
				$SQL     = "
                  SELECT summary_date, acct_id, sum(click_count) as scount, sum(click_amount_net) as commission FROM $affsummary
                   WHERE acct_id IN ('$accounts')
                   AND summary_date BETWEEN '$start_search' AND '$end_search'
                   $click_commcheck
                   GROUP BY summary_date, acct_id
             ";
				$results = $oSQL->search ( array ( "SQL" => $SQL ) );
				foreach ( $results as $record ) {
						$date                                          = formatdate ( $record[ summary_date ] );
						$raw_date                                      = $record[ summary_date ];
						$affid                                         = $record[ acct_id ];
						$report_lines[ $raw_date ][ $affid ][ clicks ] = $record[ commission ];
				}
				$rpt_detail = '';
				foreach ( $report_lines as $date => $aff ) {
						$showdate          = formatdate ( $date );
						$date_search_total = 0;
						$date_click_total  = 0;
						$date_rev_total    = 0;
						foreach ( $report_lines[ $date ] as $affiliate_id => $affinfo ) {
								$report_lines[ $date ][ $affiliate_id ][ searches ] = sprintf ( '%5.3f' , $report_lines[ $date ][ $affiliate_id ][ searches ] );
								$report_lines[ $date ][ $affiliate_id ][ clicks ]   = sprintf ( '%5.3f' , $report_lines[ $date ][ $affiliate_id ][ clicks ] );
								$report_lines[ $date ][ $affiliate_id ][ revshare ] = sprintf ( '%5.3f' , $report_lines[ $date ][ $affiliate_id ][ revshare ] );
								$date_search_total += $report_lines[ $date ][ $affiliate_id ][ searches ];
								$date_click_total += $report_lines[ $date ][ $affiliate_id ][ clicks ];
								$date_rev_total += $report_lines[ $date ][ $affiliate_id ][ revshare ];
								$rpt_search_total += $report_lines[ $date ][ $affiliate_id ][ searches ];
								$rpt_click_total += $report_lines[ $date ][ $affiliate_id ][ clicks ];
								$rpt_rev_total += $report_lines[ $date ][ $affiliate_id ][ revshare ];
								$searches = $report_lines[ $date ][ $affiliate_id ][ searches ];
								$revshare = $report_lines[ $date ][ $affiliate_id ][ revshare ];
								$clicks   = $report_lines[ $date ][ $affiliate_id ][ clicks ];
								$rpt_detail .= "
                    <tr>
                       <td>
                         $showdate
                       </td>
                       <td>
                         $affiliate_id
                       </td>
                       <td align='right'>
                         $language[dollar_sign] $searches
                       </td>
                       <td align='right'>
                         $language[dollar_sign] $revshare
                       </td>
                       <td align='right'>
                         $language[dollar_sign] $clicks
                       </td>
                    </tr>
                ";
						}
						if ( !$CGI[ acct_id ] ) {
								$date_search_total = sprintf ( "%5.3f" , $date_search_total );
								$date_rev_total    = sprintf ( "%5.3f" , $date_rev_total );
								$date_click_total  = sprintf ( "%5.3f" , $date_click_total );
								$rpt_detail .= "
                    <tr>
                       <td colspan='2' align='right' style='border-top:1px solid #e0e0e0'>
                         $language[report_subtotal]
                       </td>
                       <td align='right' style='border-top:1px solid #e0e0e0'>
                         $language[dollar_sign] $date_search_total
                       </td>
                       <td align='right' style='border-top:1px solid #e0e0e0'>
                         $language[dollar_sign] $date_rev_total
                       </td>
                       <td align='right' style='border-top:1px solid #e0e0e0'>
                         $language[dollar_sign] $date_click_total
                       </td>
                    </tr>
                    <tr><td colspan='5'>&nbsp;</td></tr>
                ";
						}
				}
				$rpt_search_total = sprintf ( "%5.3f" , $rpt_search_total );
				$rpt_rev_total    = sprintf ( "%5.3f" , $rpt_rev_total );
				$rpt_click_total  = sprintf ( "%5.3f" , $rpt_click_total );
				$grand_total      = sprintf ( "%5.3f" , $rpt_search_total + $rpt_click_total + $rpt_rev_total );
				$report           = "
             <b><u>$CGI[acct_id]</u></b>
             <blockquote>

             <link rel='stylesheet' href='$config[include_dir]/tablesort.css'>
             <table summary='x1' class='sorted' border='0' cellspacing='1' cellpadding='5'>
               <thead>
               <tr>
                 <td>$language[affiliate_report_date]</td>
                 <td>$language[report_affiliate_link]</td>
                 <td>$language[affiliate_report_searches]</td>
                 <td>$language[affiliate_report_revshare]</td>
                 <td>$language[affiliate_report_clicks]</td>
               </tr>
               </thead>

               <tbody>
               $rpt_detail

               <tr>
                   <td colspan='2' align='right' style='border-top:1px solid #000000'>
                     $language[total_due]
                   </td>
                   <td align='right' style='border-top:1px solid #000000'>
                     $language[dollar_sign] $rpt_search_total
                   </td>
                   <td align='right' style='border-top:1px solid #000000'>
                     $language[dollar_sign] $rpt_rev_total
                   </td>
                   <td align='right' style='border-top:1px solid #000000'>
                     $language[dollar_sign] $rpt_click_total
                   </td>
               </tr>

               <tr><td colspan='5'>&nbsp;</td></tr>

               <tr>
                   <td colspan=5 align='right'>
                       <b>$language[total_due]: $language[dollar_sign] $grand_total</b>
                   </td>

               </tr>
               </tbody>

             </table>
             </blockquote>
         ";

				return $report;
		}

		function affiliate_performance_detail ()
		{
                   
				global $SCRIPT_NAME;
				global $language;
				global $oSQL;
				global $CGI;
				global $config;
				$final_report = '';
				$grand_total  = '';
				$accounts     = array ();
				if ( !$CGI[ paid_flag ] ) {
						$CGI[ paid_flag ] = "unpaid";
				}
				if ( $CGI[ acct_id ] ) {
						array_push ( $accounts , $CGI[ acct_id ] );
				} else {
						$SQL        = "SELECT affiliate FROM " . AFF_INFO_TABLE;
						$affiliates = $oSQL->search ( array ( "SQL" => $SQL ) );
						foreach ( $affiliates as $affiliate ) {
								array_push ( $accounts , $affiliate[ affiliate ] );
						}
				}
				foreach ( $accounts as $affiliate ) {
						$account = new Account( array ( "Parent" => $oSQL , "Table" => ACCOUNTS_TABLE , "Schema" => ACCOUNTS_TABLE , "Lookup" => 1 , "acct_id" => $affiliate ) );
						list ( $total , $report ) = $account->affiliate_performance_detail ( array ( "date" => $CGI[ date ] , "paid_flag" => $CGI[ paid_flag ] ) );
						$this_report = "
              <b>$affiliate</b><br /><br />
              $report
            ";
						$final_report .= $this_report;
						$grand_total += $total;
				}
				$final_report .= " <h4>$language[aff_report_grand_total] <font color='green'>$language[dollar_sign] $grand_total</font></h4> ";

				return ( $final_report );
		}

		function affiliate_payments ()
		{
				global $SCRIPT_NAME;
				global $language;
				global $oSQL;
				global $CGI;
				global $config;
				$CGI[ acct_id ] = $CGI[ affiliate ];
				$report_form    = jackhammer_report_form ( "Payments" );
				admin_output ( "$language[aff_payment_report_title] $CGI[affiliate]" , $report_form );
		}


		function affiliate_payment_report ( $total_only = '' )
		{
                     
				global $SCRIPT_NAME;
				global $language;
				global $oSQL;
				global $CGI;
				global $config;
				$final_report = '';
				$grand_total  = '';
				$rows         = '';
				$accounts     = array ();
				$starttime    = mktime ( 0 , 0 , 0 , $CGI[ start_month ] , $CGI[ start_day ] , $CGI[ start_year ] );
				$endtime      = mktime ( 23 , 59 , 59 , $CGI[ end_month ] , $CGI[ end_day ] , $CGI[ end_year ] );
				if ( $CGI[ 'affiliate_id' ] ) {
						array_push ( $accounts , $CGI[ 'affiliate_id' ] );
				} else {
						$SQL        = "SELECT affiliate FROM " . AFF_INFO_TABLE;
						$affiliates = $oSQL->search ( array ( "SQL" => $SQL ) );
						foreach ( $affiliates as $affiliate ) {
								array_push ( $accounts , $affiliate[ affiliate ] );
						}
				}
				foreach ( $accounts as $affiliate ) {
						if ( !$affiliate ) {
								continue;
						}
						$account = new Account( array ( "Parent" => $oSQL , "Table" => ACCOUNTS_TABLE , "Schema" => ACCOUNTS_TABLE , "Lookup" => 1 , "acct_id" => $affiliate ) );
						list ( $report , $total , $s , $t ) = $account->generate_affiliate_payment_report ( array ( "end_day" => $CGI[ 'end_day' ] , "end_month" => $CGI[ 'end_month' ] , "end_year" => $CGI[ 'end_year' ] , "start_day" => $CGI[ 'start_day' ] , "start_month" => $CGI[ 'start_month' ] , "start_year" => $CGI[ 'start_year' ] , "minimum" => $CGI[ 'minimum' ] ) );
						$grand_total += $total;
						$rows .= $report;
				}
				$final_report = "
           <table width='600'>
            <tr>
             <td>
               <b>$CGI[acct_id]</b> $language[aff_payment_report_ending] $CGI[end_month]/$CGI[end_day]/$CGI[end_year]

               <br /><br />
               $language[aff_payment_report_header]
               <br /><br />

               <form action='$SCRIPT_NAME' method='post'>
                   <table border=0 cellspacing=1 bgcolor='#e0e0e0'>
                    <tr bgcolor='black'>
                       <th><font color='white'>$language[aff_payment_report_pay]</font></th>
                       <th><font color='white'>$language[aff_payment_report_affiliate]</font></th>
                       <th><font color='white'>$language[aff_payment_report_due]</font></th>
                       <th><font color='white'>$language[aff_payment_report_comments]</font></th>
                    </tr>
                    $rows
                   </table>

                   <input type='hidden' name='action' value='pay_affiliates'>
                   <input type='hidden' name='admin' value='hyperseek'>
                   <input type='hidden' name='start_date' value='$CGI[start_year]$CGI[start_month]$CGI[start_day]'>
                   <input type='hidden' name='end_date' value='$CGI[end_year]$CGI[end_month]$CGI[end_day]'>
                   <input type='hidden' name='starttime' value='$starttime'>
                   <input type='hidden' name='endtime' value='$endtime'>

                   <p>
                   $language[aff_payment_report_mark_paid] <input name='paid_date' value='$CGI[end_month]/$CGI[end_day]/$CGI[end_year]'>
                   <p>
                   <center><input type='submit' value='$language[aff_payment_report_button]'></center>
               </form>
              <h4>$language[aff_report_grand_total] <font color='green'>$language[dollar_sign] $grand_total</font></h4>
            </td>
           </tr>
          </table>
       ";
				if ( $total_only ) {
						return $grand_total;
				} else {
						return ( $final_report );
				}
		}


		function pay_affiliates ()
		{
                    
				global $SCRIPT_NAME;
				global $language;
				global $oSQL;
				global $iSQL;
				global $CGI;
				global $config;
				global $data_dir;
				$report       = '';
				$tab          = '';
				$csv          = '';
				$text         = '';
				$timestamp    = time ();
				$paid_date    = $CGI[ 'paid_date' ];
				$period_start = $CGI[ 'start_date' ];
				$period_end   = $CGI[ 'end_date' ];
				$starttime    = $CGI[ 'starttime' ];
				$endtime      = $CGI[ 'endtime' ];
				$report       = '';
				$paypal       = '';
				$tab          = '';
				$csv          = '';
				$text         = '';
				$ahash        = array ();
				$alist        = array ();
				$revsharelist = array ();
				// Table Vars
				$accounts = ACCOUNTS_TABLE;
				$affinfo  = AFF_INFO_TABLE;
				foreach ( $CGI as $key => $value ) {
						if ( !preg_match ( '/:/' , $key ) ) {
								continue;
						}
						list ( $affiliate , $act ) = split ( ':' , $key );
						$ahash[ $affiliate ][ $act ] = $value;
				}
				foreach ( $ahash as $affiliate => $vals ) {
						if ( $ahash[ $affiliate ][ pay ] ) {
								$SQL                                    = "
               SELECT $accounts.*, $affinfo.* FROM $accounts, $affinfo
               WHERE $accounts.acct_id = '$affiliate'
               AND $affinfo.affiliate = $accounts.acct_id
             ";
								$record                                 = $oSQL->search ( array ( "SQL" => $SQL , "Return" => 'hash' ) );
								$balance                                = ( $ahash[ $affiliate ][ balance ] + $ahash[ $affiliate ][ original_amount ] ) - $ahash[ $affiliate ][ amount ];
								$balance                                = sprintf ( "%5.3f" , ( $balance ) );
								$ahash[ $affiliate ][ amount ]          = sprintf ( "%5.3f" , ( $ahash[ $affiliate ][ amount ] ) );
								$ahash[ $affiliate ][ original_amount ] = sprintf ( "%5.3f" , ( $ahash[ $affiliate ][ original_amount ] ) );
								$payment                                = new Record( array ( "Parent" => $iSQL , "Table" => AFF_PAYMENTS_TABLE , "Schema" => AFF_PAYMENTS_TABLE ) );
								$payment->data[ affiliate ]             = $affiliate;
								$payment->data[ paid_date ]             = $paid_date;
								$payment->data[ paid_amount ]           = $ahash[ $affiliate ][ amount ];
								$payment->data[ due_amount ]            = $ahash[ $affiliate ][ original_amount ];
								$payment->data[ balance ]               = $balance;
								$payment->data[ period_start ]          = $period_start;
								$payment->data[ period_end ]            = $period_end;
								$payment->data[ tstamp ]                = $timestamp;
								$payment->data[ comments ]              = $ahash[ $affiliate ][ comments ];
								$payment->update_values ( $payment->data );
								$payment->save ();
								## If payment method (payment_type from affiliate_info) = "deposit", then insert a record into acct_transactions, and update their balance.
								if ( preg_match ( "/deposit/i" , $record[ payment_type ] ) ) {
										$SQL = "UPDATE " . ACCOUNTS_TABLE . " SET current_balance = current_balance + " . $ahash[ $affiliate ][ 'amount' ] . "WHERE acct_id = '$affiliate'";
										$iSQL->run ( $SQL );
										## Finance Table Record
										$finances                       = new Record( array ( "Parent" => $iSQL , "Table" => TRANSACTIONS_TABLE , "Schema" => TRANSACTIONS_TABLE ) );
										$finances->data[ acct_id ]      = $affiliate;
										$finances->data[ source ]       = $language[ deposit_message_affiliate ];
										$finances->data[ trans_amount ] = $ahash[ $affiliate ][ amount ];
										$finances->data[ trans_date ]   = $timestamp;
										$finances->data[ trans_type ]   = "PAYMENT";
										$finances->data[ tax_amount ]   = 0.00;
										$finances->data[ affiliate ]    = "";
										$finances->data[ extra_info ]   = "";
										$finances->update_values ( $finances->data );
										$finances->save ();
										## Update the acct_summary Table Record
										$summary                        = new Record( array ( "Parent" => $iSQL , "Table" => ACCT_SUMMARY_TABLE , "Schema" => ACCT_SUMMARY_TABLE , "Lookup" => 1 , "acct_id" => $affiliate ) );
										$new_total                      = $summary->data[ earnings_paid ] + $ahash[ $affiliate ][ amount ];
										$summary->data[ acct_id ]       = $affiliate;
										$summary->data[ earnings_paid ] = $new_total;
										$summary->update_values ( $summary->data );
										$summary->save ();
								}
								## Prepare for the big updates ##
								array_push ( $alist , $affiliate );
								echo ".";
								if ( preg_match ( "/revshare/i" , $record[ payment_trigger ] ) ) {
										echo "Revshare array<BR>";
										array_push ( $revsharelist , $affiliate );
								}
								## Now, increment the report, and the file contents.
								if ( $ahash[ $affiliate ][ amount ] >= 0.01 ) {
										$amt = $ahash[ $affiliate ][ amount ];
										$cmt = $ahash[ $affiliate ][ comments ];
										$report .= "<B>$affiliate</B> ... $language[aff_payment_report_paid] \$ $amt $language[aff_payment_report_via] $record[payment_type]<br /> Admin Comments: $cmt<br /><br />\n";
										$tab .= "$record[acct_id]\t$record[contact_email]\t$record[contact_name]\t$record[address]\t$record[city]\t$record[state]\t$record[zip]\t$record[phone]\t$record[fax]\t$period_start\t$period_end\t$paid_date\t$amt\t$record[payment_type]\t$balance\n";
										$paypal .= "$record[contact_email]\t$amt\t$record[acct_id]\t$cmt\n";
										$csv .= "'$record[acct_id]','$record[contact_email]','$record[contact_name]','$record[address]','$record[city]','$record[state]','$record[zip]','$record[phone]','$record[fax]','$period_start','$period_end','$paid_date','$amt','$record[payment_type]','$balance'\n";
										$text .= "$language[aff_payment_report_payee]: 	$record[contact_name]  (Account ID: $record[acct_id]) \n ";
										$text .= "           $record[contact_email] \n ";
										$text .= "           $record[address] \n ";
										$text .= "           $record[city], $record[state] $record[zip] \n ";
										$text .= "           $language[aff_payment_report_phone]: $record[phone] / $language[aff_payment_report_fax]: $record[fax] \n\n ";
										$text .= "   $language[aff_payment_report_paid]: \$$amt  $language[aff_payment_report_via] $record[payment_type] \n ";
										$text .= "   $language[aff_payment_report_balance]: \$$balance \n ";
										$text .= "============================================================================= \n\n ";
								}
						} else {
								$report .= "<B>$record[acct_id]</B> ... $language[aff_payment_report_skipped]<br />\n";
						}
				}
				### Create hard copies
				mkdir ( "$data_dir/hyperseek/affiliates/" , 0777 );
				$filename     = cleanup_string ( $paid_date , "_" , 1 );
				$tab_filename = "$data_dir/hyperseek/affiliates/$filename.tab";
				$tab_contents = "$language[aff_payment_report_account]\t$language[aff_payment_report_email]\t$language[aff_payment_report_name]\t$language[aff_payment_report_address]\t$language[aff_payment_report_city]\t$language[aff_payment_report_state]\t$language[aff_payment_report_zip]\t$language[aff_payment_report_phone]\t$language[aff_payment_report_fax]\t$language[aff_payment_report_start]\t$language[aff_payment_report_end]\t$language[aff_payment_report_paid_date]\t$language[aff_payment_report_paid]\t$language[aff_payment_report_via]\t$language[aff_payment_report_balance]\n" . $tab . "$language[aff_payment_report_endbatch]\n\n";
				write_file ( $tab_filename , $tab_contents );
				$csv_filename = "$data_dir/hyperseek/affiliates/$filename.csv";
				$csv_contents = " '$language[aff_payment_report_account]','$language[aff_payment_report_email]','$language[aff_payment_report_name]','$language[aff_payment_report_address]','$language[aff_payment_report_city]','$language[aff_payment_report_state]','$language[aff_payment_report_zip]','$language[aff_payment_report_phone]','$language[aff_payment_report_fax]','$language[aff_payment_report_start]','$language[aff_payment_report_end]','$language[aff_payment_report_paid_date]','$language[aff_payment_report_paid]','$language[aff_payment_report_via]','$language[aff_payment_report_balance]'\n" . $csv . "$language[aff_payment_report_endbatch]\n\n";
				write_file ( $csv_filename , $csv_contents );
				$txt_filename = "$data_dir/hyperseek/affiliates/$filename.txt";
				$txt_contents = "$period_start -> $period_end  (Actual Paid Date: $paid_date)\n\n" . $text . "$language[aff_payment_report_endbatch]\n\n";
				write_file ( $txt_filename , $txt_contents );
				$paypal_filename = "$data_dir/hyperseek/affiliates/$filename.tab";
				write_file ( $paypal_filename , $paypal );
				echo "<br />Running Mass Update Queries....<br /><br />";
				### Massive Table Updates ###
				$paylist1 = implode ( "','" , $alist );
				$paylist2 = implode ( "','" , $revsharelist );
				if ( $paylist1 ) {
						$SQL = "UPDATE " . AFF_SUMMARY_TABLE . " SET paid_flag = 'YES' WHERE acct_id IN('$paylist1') AND summary_date BETWEEN '$period_start' AND '$period_end'";
						$iSQL->run ( $SQL );
						$SQL = "UPDATE " . AFF_SEARCHES_TABLE . " SET paid_flag = 'YES' WHERE acct_id IN('$paylist1') AND search_date BETWEEN '$period_start' AND '$period_end'";
						$iSQL->run ( $SQL );
						$SQL = "UPDATE " . CLICK_LOG_TABLE . " SET paid_flag = 'YES' WHERE hit_date BETWEEN '$period_start' AND '$period_end' AND affiliate IN('$paylist1')";
						$iSQL->run ( $SQL );
				}
				if ( $paylist2 ) {
						$SQL = "UPDATE " . TRANSACTIONS_TABLE . " SET commission_paid_date = $period_end WHERE affiliate IN('$paylist2') AND trans_date BETWEEN '$starttime' AND '$endtime'";
						$iSQL->run ( $SQL );
				}
				######### OUTPUT IT ###########
				$OUTPUT = "
          <table border='0' width='600'><tr><td>
            $report

            $language[aff_payment_report_files] $data_dir/hyperseek/affiliates:
            <ul>
               <li><b>$filename.tab</b>: $language[aff_payment_report_tab]
               <li><b>$filename.csv</b>: $language[aff_payment_report_csv]
               <li><b>$filename.txt</b>: $language[aff_payment_report_text]
            </ul>
            </td>
            </tr>
          </table>
          <br />
       ";
				admin_output ( $language[ aff_payment_report_title ] , $OUTPUT );
		}


		function affiliate_audit ()
		{
				global $SCRIPT_NAME;
				global $language;
				global $CGI;
				global $config;
				global $data_dir;
				$file = "$data_dir/hyperseek/affiliates/adjust.rpt";
				if ( file_exists ( $file ) ) {
						$report = implode ( "" , file ( $file ) );
						admin_output ( $language[ aff_audit_report_title ] , $report );
				} else {
						admin_output ( $language[ aff_audit_report_title ] , $report );
				}
		}

?>
