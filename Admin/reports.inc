<?php

		/*

		________________________________________________________________________________

			PROGRAM : Hyperseek Core Admin Center
			VERSION : 1.00
			CVS     : $REVISION$

			Author  : John Cokos, iWeb, Inc.

			Purpose :

		________________________________________________________________________________


												_______/\/\______________________/\/\__/\/\____/\/\_
												_______/\/\____/\/\/\/\________/\/\______/\/\/\/\___
												___/\/\/\/\__/\/\/\/\________/\/\__________/\/\_____
												_/\/\__/\/\________/\/\____/\/\__________/\/\/\/\___
												___/\/\/\/\__/\/\/\/\____/\/\__________/\/\____/\/\_
												____________________________________________________


																																ds/X: Perl/PHP/ASP/mySQL API (c) 2001 iWeb, Inc.

		________________________________________________________________________________
				Copyright and Legal Notices:

						All source code, images, programs, files included in this distribution
						Copyright (c) 1996 - 2003

													John C. Cokos, The CCS Network, Inc.  iWeb, Inc.
													All Rights Reserved.

						Use, distibution, sale, or access to this program is forbidden without
						permission, except were permitted by the license agreement.

		________________________________________________________________________________
				Special Instructions and notes...

		________________________________________________________________________________

		*/

		// ------------------------------------------------------------------------------- //
		// REPORT FORMS
		// ------------------------------------------------------------------------------- //

		function jackhammer_report_form ( $aff = '' )
		{
				global $CGI;
				global $SCRIPT_NAME;
				global $language;
				$day       = date ( "d" );
				$month     = date ( "m" );
				$year      = date ( "Y" );
				$next_year = $year + 1;
				$last_year = $year - 1;
				if ( $CGI[ start_month ] ) {
						$start_month_option = "<option value='$CGI[start_month]'>$CGI[start_month]</option>";
				}
				if ( $CGI[ end_month ] ) {
						$end_month_option = "<option value='$CGI[end_month]'>$CGI[end_month]</option>";
				}
				$month_options = "<option value='$month'>$month</option>\n";
				for ( $m = 1; $m <= 12; $m++ ) {
						$x = $m;
						$x = sprintf ( "%02d" , $x );
						$month_options .= "<option value='$x'>$x</option>\n";
				}
				if ( $CGI[ start_day ] ) {
						$start_day_option = "<option value='$CGI[start_day]'>$CGI[start_day]</option>";
				}
				if ( $CGI[ end_day ] ) {
						$end_day_option = "<option value='$CGI[end_day]'>$CGI[end_day]</option>\n";
				}
				$day_options = "<option value='$day'>$day</option>\n";
				for ( $d = 1; $d <= 31; $d++ ) {
						$x = $d;
						$x = sprintf ( "%02d" , $x );
						$day_options .= "<option value='$x'>$x</option>\n";
				}
				if ( $CGI[ start_year ] ) {
						$end_year_option = "<option value='$CGI[start_year]'>$CGI[start_year]</option>";
				}
				if ( $CGI[ end_year ] ) {
						$end_year_option = "<option value='$CGI[end_year]'>$CGI[end_year]</option>\n";
				}
				$year_options = "
         <option value='$year'>$year</option>
         <option value='$last_year'>$last_year</option>
         <option value='$next_year'>$next_year</option>
       ";
				if ( $aff == "Payments" ) {
						$CGI[ start_month ] = 1;
						$CGI[ start_year ]  = 2000;
						$CGI[ start_day ]   = 1;
				} elseif ( !$CGI[ start_month ] ) {
						$CGI[ start_month ] = $mon;
						$CGI[ start_year ]  = $year;
						$CGI[ start_day ]   = $mday;
				}
				if ( !$CGI[ end_month ] ) {
						$CGI[ end_month ] = $mon;
						$CGI[ end_year ]  = $year;
						$CGI[ end_day ]   = $mday;
				}
				$curr_type = '';
				if ( $CGI[ report_type ] ) {
						$curr_type = " <option value='$CGI[report_type]' selected='selected'>$CGI[report_type]</option> ";
				}
				$type_options      = "
            $curr_type
            <option value='click_summary'>$language[report_type_summary]</option>
            <option value='fraud_summary'>$language[report_type_fraud]</option>
       ";
				$breakdown_options = "
            <option value='$CGI[breakdown]' selected='selected'>$CGI[breakdown]</option>
            <option value=''>None</option>
            <option value='affiliate'>$language[report_breakdown_affiliate]</option>
            <option value='source'>$language[report_breakdown_source]</option>
       ";
				$account_input     = "
           <a href='javascript:accountList(\"acct_id\")'>$language[report_account_link]</a>:&nbsp;
           <input name='acct_id' value='$CGI[acct_id]' size='8' />
       ";
				$affiliate_input   = "
           <a href='javascript:accountList(\"affiliate_id\")'>$language[report_affiliate_link]</A>:&nbsp;
           <input name='affiliate_id' value='$CGI[affiliate_id]' size='8' />
       ";
				$source_input      = "
           <a href='javascript:accountList(\"source\")'>$language[report_source_link]</a>:&nbsp;
           <input name='source' value='$CGI[source]' size='8' />
       ";
				$breakdown_form    = "
              <tr>
                <td>$language[report_breakdown]</td>
                <td valign='top' colspan='3' nowrap='nowrap'>
                    <select name='breakdown'>
                        $breakdown_options
                    </select>
                </td>
              </tr>
       ";
				## Spreadsheet Report -- Force type and hide some options
				if ( $aff == "excel" ) {
						$type_options    = "<option value='excel'>Spreadsheet</option>";
						$affiliate_input = '';
						$account_input   = '';
						$source_input    = '';
						$breakdown_form  = '';
				} ## Affiliate Payment Report: disable some options ##
				elseif ( $aff ) {
						$type_options   = "
            <option value='Payments' selected='selected'>Payments Due Form</option>
            <option value='AffiliateSummary'>Summary</option>
           ";
						$account_input  = '';
						$source_input   = '';
						$breakdown_form = '';
				}
				## Financial Summary Report: disable some options ##
				if ( $CGI[ report_type ] == "financial_summary" ) {
						$type_options    = "
            <option value='financial_summary' selected='selected'>$language[report_financial_summary]</option>
           ";
						$affiliate_input = '';
						$account_input   = '';
						$source_input    = '';
						$breakdown_form  = '';
				}
				## Financial Summary Report: disable some options ##
				if ( $CGI[ report_type ] == "trend_report" ) {
						$type_options   = "
            <option value='trend_report' selected='selected'>$language[trend_report_prompt]</option>
           ";
						$breakdown_form = "
              <tr>
                <td>$language[report_breakdown]</td>
                <td valign='top' colspan='3' nowrap='nowrap'>
                    <select name='breakdown'>
                        <option value='ip'>IP Address</option>
                        <option value='user_agent'>User Agent</option>
                        <option value='referer'>Referer</option>
                        <option value='interval'>Interval/Time</option>
                    </select>
                </td>
              </tr>
              <tr>
                <td>$language[trend_minimum]</td>
                <td valign='top' colspan='3' nowrap='nowrap'>
                    <input name='minimum' size='4' />
                </td>
              </tr>
           ";
				}

				return "
          <script language='javascript'>
            function accountList(type) {
               var URL = '$SCRIPT_NAME?admin=hyperseek&action=pick_account&type=' + type;
               X=window.open(URL,'X','toolbar=no,directories=no,menubar=no,scrollbars=no,resizable=yes,width=400,height=150');
            }
          </script>
          <center>
          <form name='report_form' action='$SCRIPT_NAME' method='get' target='main'>
              <table border=0>

              <tr>
                <td colspan='4' norwap='nowrap'>
                    $account_input
                    $affiliate_input
                    $source_input
                </td>
              </tr>

              <tr>
                <td>$language[report_type]</td>
                <td valign='top' colspan='3' nowrap='nowrap'>
                    <select name='report_type'>
                        $type_options
                    </select>
                </td>
              </tr>

              $breakdown_form



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
                 <input type='hidden' name='action' value='report'>
                 <input type='hidden' name='admin' value='hyperseek'>
                 <center><input type='submit' value='$language[report_button]'></center>
              </tr>
              </table>
          </form>
          </center>
       ";
		}


		function excel_report_form ()
		{
				global $language;
				$report_form = jackhammer_report_form ( "excel" );
				admin_output ( "Downloadable PPC Summary" , $report_form );
		}

		function click_report_form ()
		{
				global $language;
				$report_form = jackhammer_report_form ();
				admin_output ( $language[ click_report ] , $report_form );
		}

		function trend_report_form ()
		{
				global $language;
				$report_form = jackhammer_report_form ();
				admin_output ( $language[ trend_report ] , $report_form );
				$report_form = jackhammer_report_form ();
		}


		function report ()
		{
				global $SCRIPT_NAME;
				global $language;
				global $oSQL;
				global $CGI;
				global $config;
				$account = $CGI[ acct_id ];
				if ( !$account ) {
						$account = "All Accounts";
				}
				if ( $CGI[ report_type ] == "click_summary" ) {
						$title  = "Click Summary";
						$report = click_report ();
				} elseif ( $CGI[ report_type ] == "click_detail" ) {
						$title  = "Daily Detail";
						$report = click_report_detail ();
				} elseif ( $CGI[ report_type ] == "keyword_detail" ) {
						$title  = "Keyword Detail";
						$report = keyword_report_detail ();
				} elseif ( $CGI[ report_type ] == "view_click" ) {
						$title  = "Single Click Detail";
						$report = view_click ();
				} elseif ( $CGI[ report_type ] == "view_fraud" ) {
						$title  = "Single Fraud Click Detail";
						$report = view_fraud ();
				} elseif ( $CGI[ report_type ] == "financial_summary" ) {
						$title  = "Financial Summary";
						$report = financial_summary ();
				} elseif ( $CGI[ report_type ] == "fraud_summary" ) {
						$title  = "Fraud Summary";
						$report = fraud_report ();
				} elseif ( $CGI[ report_type ] == "fraud_list" ) {
						$title  = "Fraud Listing";
						$report = fraud_list ();
				} elseif ( $CGI[ report_type ] == "trend_report" ) {
						$title  = "Trend Report";
						$report = trend_report ();
				} elseif ( $CGI[ report_type ] == "excel" ) {
						$title  = "Downloadable Report";
						$report = spreadsheet_report ();
				} ## Aff Payments ##
				elseif ( $CGI[ report_type ] == "Payments" ) {
						$title  = "Payment Report: $account";
						$report = affiliate_payment_report ();
				} elseif ( $CGI[ report_type ] == "AffiliateSummary" ) {
						$title  = "Affiliate Summary Report: $account";
						$report = affiliate_performance_report ();
				} elseif ( $CGI[ report_type ] == "affdetail" ) {
						$title  = "Affiliate Detail Report: $account";
						$report = affiliate_performance_detail ();
				} else {
						$report = "Unknown Report Type: [<b>$CGI[report_type]</b>] Specified";
				}
				admin_output ( $title , $report );
		}

		function spreadsheet_report ()
		{
				global $oSQL;
				global $CGI;
				global $data_dir;
				$click_log = CLICK_LOG_TABLE;
				$fname     = $data_dir . "/financial-report.xls";
				$savename  = "financial-report.xls";
				include_once ( "dsX/Addons/write-excel.inc" );
				// Set up the spreadsheet //
				$workbook = & new writeexcel_workbook( $fname );
				## Formats
				$unlocked           =& $workbook->addformat ( array ( locked => 0 ) );
				$locked             =& $workbook->addformat ( array ( locked => 1 ) );
				$format_bold        =& $workbook->addformat ( array ( bold => 1 ) );
				$format_header      =& $workbook->addformat ( array ( bold => 1 , align => 'center' , bg_color => 'silver' , color => 'navy' , size => 12 ) );
				$dollar_format      =& $workbook->addformat ( array ( num_format => '$#,##0.00' ) );
				$count_format       =& $workbook->addformat ( array ( num_format => '#,##0' ) );
				$bold_dollar_format =& $workbook->addformat ( array ( bold => 1 , num_format => '$#,##0.00' ) );
				$bold_count_format  =& $workbook->addformat ( array ( bold => 1 , num_format => '#,##0' ) );
				$end_day            = $CGI[ end_day ];
				$end_month          = $CGI[ end_month ];
				$end_year           = $CGI[ end_year ];
				$start_day          = $CGI[ start_day ];
				$start_month        = $CGI[ start_month ];
				$start_year         = $CGI[ start_year ];
				$start_date         = $start_year . $start_month . $start_day;
				$end_date           = $end_year . $end_month . $end_day;
				// Daily Summary //
				$SQL       = "SELECT * FROM " . DAILY_SUMMARY_TABLE . " WHERE summary_date BETWEEN $start_date AND $end_date ORDER BY summary_date ASC";
				$dailydata = $oSQL->search ( array ( "SQL" => $SQL ) );
				// Acct Summary //
				$SQL      = "SELECT click_date, acct_id, sum(click_count) as click_count, sum(click_amount_net) as click_amount_net, sum(click_amount_gross) as click_amount_gross, sum(fraud_count) as fraud_count FROM " . CLICK_SUMMARY_TABLE . " WHERE click_date BETWEEN $start_date AND $end_date GROUP BY acct_id, click_date ORDER BY click_date ASC, acct_id ASC";
				$acctdata = $oSQL->search ( array ( "SQL" => $SQL ) );
				// Affiliate Summary //
				$SQL     = "SELECT * FROM " . AFF_SUMMARY_TABLE . " WHERE summary_date BETWEEN $start_date AND $end_date ORDER BY summary_date ASC, acct_id ASC";
				$affdata = $oSQL->search ( array ( "SQL" => $SQL ) );
				// Source Summary //
				$SQL        = "SELECT * FROM " . SOURCE_SUMMARY_TABLE . " WHERE summary_date BETWEEN $start_date AND $end_date ORDER BY summary_date ASC, source ASC";
				$sourcedata = $oSQL->search ( array ( "SQL" => $SQL ) );
				// Search Terms Summary //
				$SQL        = "SELECT * FROM " . LOG_SUMMARY_TABLE . " WHERE search_type in('TEXTSEARCH','') ORDER BY criteria ASC";
				$searchdata = $oSQL->search ( array ( "SQL" => $SQL ) );
				// Category Summary //
				$SQL          = "SELECT * FROM " . LOG_SUMMARY_TABLE . " WHERE search_type='cat' ORDER BY criteria ASC";
				$categorydata = $oSQL->search ( array ( "SQL" => $SQL ) );
				// Browser Summary //
				$SQL         = "
            SELECT user_agent AS criteria, count(*) AS cnt FROM $click_log
            WHERE hit_date BETWEEN $start_date AND $end_date
            GROUP BY user_agent
            HAVING count(*) >= 5
            ORDER BY user_agent
       ";
				$browserdata = $oSQL->search ( array ( "SQL" => $SQL ) );
				// IP Summary //
				$SQL    = "
            SELECT ip AS criteria, count(*) AS cnt FROM $click_log
            WHERE hit_date BETWEEN $start_date AND $end_date
            GROUP BY ip
            HAVING count(*) >= 5
            ORDER BY ip
       ";
				$ipdata = $oSQL->search ( array ( "SQL" => $SQL ) );
				// DAILY (OVERALL) Summary
				# Add a worksheet
				$worksheet = & $workbook->addworksheet ( "Daily Summary" );
				# Create Headers
				$worksheet->write ( "A1" , "Date" , $format_header );
				$worksheet->write ( "B1" , "# Clicks" , $format_header );
				$worksheet->write ( "C1" , "Click $ (Gross)" , $format_header );
				$worksheet->write ( "D1" , "Click $ (Net)" , $format_header );
				$worksheet->write ( "E1" , "# Searches" , $format_header );
				$worksheet->write ( "F1" , "Search $" , $format_header );
				$worksheet->write ( "G1" , "# Fraud" , $format_header );
				$row = 1;
				foreach ( $dailydata as $dailyrecord ) {
						$d_t_click_count += $dailyrecord[ click_count ];
						$d_t_click_amount_gross += $dailyrecord[ click_amount_gross ];
						$d_t_click_amount_net += $dailyrecord[ click_amount_net ];
						$d_t_search_count += $dailyrecord[ search_count ];
						$d_t_search_amount += $dailyrecord[ search_amount ];
						$d_t_fraud_count += $dailyrecord[ fraud_count ];
						$date = formatdate ( $dailyrecord[ 'summary_date' ] , "m/d/Y" );
						$worksheet->write ( $row , 0 , $date );
						$worksheet->write ( $row , 1 , $dailyrecord[ click_count ] , $count_format );
						$worksheet->write ( $row , 2 , $dailyrecord[ click_amount_gross ] , $dollar_format );
						$worksheet->write ( $row , 3 , $dailyrecord[ click_amount_net ] , $dollar_format );
						$worksheet->write ( $row , 4 , $dailyrecord[ search_count ] , $count_format );
						$worksheet->write ( $row , 5 , $dailyrecord[ search_amount ] , $dollar_format );
						$worksheet->write ( $row , 6 , $dailyrecord[ fraud_count ] , $count_format );
						$row++;
				}
				$worksheet->write ( $row , 1 , $d_t_click_count , $bold_count_format );
				$worksheet->write ( $row , 2 , $d_t_click_amount_gross , $bold_dollar_format );
				$worksheet->write ( $row , 3 , $d_t_click_amount_net , $bold_dollar_format );
				$worksheet->write ( $row , 4 , $d_t_search_count , $bold_count_format );
				$worksheet->write ( $row , 5 , $d_t_search_amount , $bold_dollar_format );
				$worksheet->write ( $row , 6 , $d_t_fraud_count , $bold_count_format );
				// ACCOUNT Summary
				# Add a worksheet
				$worksheet = & $workbook->addworksheet ( "Account Summary" );
				# Create Headers
				$worksheet->write ( "A1" , "Date" , $format_header );
				$worksheet->write ( "B1" , "Account" , $format_header );
				$worksheet->write ( "C1" , "# Clicks" , $format_header );
				$worksheet->write ( "D1" , "Click $ (Gross)" , $format_header );
				$worksheet->write ( "E1" , "Click $ (Net)" , $format_header );
				$worksheet->write ( "F1" , "# Fraud" , $format_header );
				$row = 1;
				foreach ( $acctdata as $acctrecord ) {
						$acct_t_click_count += $acctrecord[ click_count ];
						$acct_t_click_amount_gross += $acctrecord[ click_amount_gross ];
						$acct_t_click_amount_net += $acctrecord[ click_amount_net ];
						$acct_t_fraud_count += $acctrecord[ fraud_count ];
						$date = formatdate ( $acctrecord[ 'click_date' ] , "m/d/Y" );
						$worksheet->write ( $row , 0 , $date );
						$worksheet->write ( $row , 1 , $acctrecord[ acct_id ] );
						$worksheet->write ( $row , 2 , $acctrecord[ click_count ] , $count_format );
						$worksheet->write ( $row , 3 , $acctrecord[ click_amount_gross ] , $dollar_format );
						$worksheet->write ( $row , 4 , $acctrecord[ click_amount_net ] , $dollar_format );
						$worksheet->write ( $row , 5 , $acctrecord[ fraud_count ] , $count_format );
						$row++;
				}
				$worksheet->write ( $row , 2 , $acct_t_click_count , $bold_count_format );
				$worksheet->write ( $row , 3 , $acct_t_click_amount_gross , $bold_dollar_format );
				$worksheet->write ( $row , 4 , $acct_t_click_amount_net , $bold_dollar_format );
				$worksheet->write ( $row , 5 , $acct_t_fraud_count , $bold_count_format );
				// AFFILIATE Summary
				# Add a worksheet
				$worksheet = & $workbook->addworksheet ( "Affiliate Summary" );
				# Create Headers
				$worksheet->write ( "A1" , "Date" , $format_header );
				$worksheet->write ( "B1" , "Affiliate" , $format_header );
				$worksheet->write ( "C1" , "# Interim Clicks" , $format_header );
				$worksheet->write ( "D1" , "# Clicks" , $format_header );
				$worksheet->write ( "E1" , "Click $ (Gross)" , $format_header );
				$worksheet->write ( "F1" , "Click $ (Net)" , $format_header );
				$worksheet->write ( "G1" , "# Referrals" , $format_header );
				$worksheet->write ( "H1" , "Referral $ (Gross)" , $format_header );
				$worksheet->write ( "I1" , "# Searches" , $format_header );
				$worksheet->write ( "J1" , "Search $ (Gross)" , $format_header );
				$worksheet->write ( "K1" , "# Fraud" , $format_header );
				$worksheet->write ( "L1" , "Paid?" , $format_header );
				$row = 1;
				foreach ( $affdata as $affrecord ) {
						$aff_t_click_count += $affrecord[ click_count ];
						$aff_t_click_amount_gross += $affrecord[ click_amount_gross ];
						$aff_t_click_amount_net += $affrecord[ click_amount_net ];
						$aff_t_fraud_count += $affrecord[ fraud_count ];
						$aff_t_interim_count += $affrecord[ interim_click_count ];
						$aff_t_ref_count += $affrecord[ referral_count ];
						$aff_t_ref_amount += $affrecord[ referral_amount ];
						$aff_t_search_count += $affrecord[ search_count ];
						$aff_t_search_amount += $affrecord[ search_amount ];
						$date = formatdate ( $affrecord[ 'summary_date' ] , "m/d/Y" );
						$worksheet->write ( $row , 0 , $date );
						$worksheet->write ( $row , 1 , $affrecord[ acct_id ] );
						$worksheet->write ( $row , 2 , $affrecord[ interim_click_count ] , $count_format );
						$worksheet->write ( $row , 3 , $affrecord[ click_count ] , $count_format );
						$worksheet->write ( $row , 4 , $affrecord[ click_amount_gross ] , $dollar_format );
						$worksheet->write ( $row , 5 , $affrecord[ click_amount_net ] , $dollar_format );
						$worksheet->write ( $row , 6 , $affrecord[ referral_count ] , $count_format );
						$worksheet->write ( $row , 7 , $affrecord[ referral_amount ] , $dollar_format );
						$worksheet->write ( $row , 8 , $affrecord[ search_count ] , $count_format );
						$worksheet->write ( $row , 9 , $affrecord[ search_amount ] , $dollar_format );
						$worksheet->write ( $row , 10 , $affrecord[ fraud_count ] , $count_format );
						$worksheet->write ( $row , 11 , $affrecord[ paid_flag ] );
						$row++;
				}
				$worksheet->write ( $row , 2 , $aff_t_interim_count , $bold_count_format );
				$worksheet->write ( $row , 3 , $aff_t_click_count , $bold_count_format );
				$worksheet->write ( $row , 4 , $aff_t_click_amount_gross , $bold_dollar_format );
				$worksheet->write ( $row , 5 , $aff_t_click_amount_net , $bold_dollar_format );
				$worksheet->write ( $row , 6 , $aff_t_ref_count , $bold_count_format );
				$worksheet->write ( $row , 7 , $aff_t_ref_amount , $bold_dollar_format );
				$worksheet->write ( $row , 8 , $aff_t_search_count , $bold_count_format );
				$worksheet->write ( $row , 9 , $aff_t_search_amount , $bold_dollar_format );
				$worksheet->write ( $row , 10 , $acct_t_fraud_count , $bold_count_format );
				// SOURCE Summary
				# Add a worksheet
				$worksheet = & $workbook->addworksheet ( "Source Summary" );
				# Create Headers
				$worksheet->write ( "A1" , "Date" , $format_header );
				$worksheet->write ( "B1" , "Source" , $format_header );
				$worksheet->write ( "C1" , "# Clicks" , $format_header );
				$worksheet->write ( "D1" , "Click $ (Gross)" , $format_header );
				$worksheet->write ( "E1" , "Click $ (Net)" , $format_header );
				$worksheet->write ( "F1" , "# Fraud" , $format_header );
				$row = 1;
				foreach ( $sourcedata as $sourcerecord ) {
						$summ_t_click_count += $sourcerecord[ click_count ];
						$summ_t_click_amount_gross += $sourcerecord[ click_amount_gross ];
						$summ_t_click_amount_net += $sourcerecord[ click_amount_net ];
						$summ_t_fraud_count += $sourcerecord[ fraud_count ];
						$date = formatdate ( $sourcerecord[ 'summary_date' ] , "m/d/Y" );
						$worksheet->write ( $row , 0 , $date );
						$worksheet->write ( $row , 1 , $sourcerecord[ source ] );
						$worksheet->write ( $row , 2 , $sourcerecord[ click_count ] , $count_format );
						$worksheet->write ( $row , 3 , $sourcerecord[ click_amount_gross ] , $dollar_format );
						$worksheet->write ( $row , 4 , $sourcerecord[ click_amount_net ] , $dollar_format );
						$worksheet->write ( $row , 5 , $sourcerecord[ fraud_count ] , $count_format );
						$row++;
				}
				$worksheet->write ( $row , 2 , $summ_t_click_count , $bold_count_format );
				$worksheet->write ( $row , 3 , $summ_t_click_amount_gross , $bold_dollar_format );
				$worksheet->write ( $row , 4 , $summ_t_click_amount_net , $bold_dollar_format );
				$worksheet->write ( $row , 5 , $summ_t_fraud_count , $bold_count_format );
				// SEARCH TERM Summary
				# Add a worksheet
				$worksheet = & $workbook->addworksheet ( "Search Summary" );
				# Create Headers
				$worksheet->write ( "A1" , "Search Term" , $format_header );
				$worksheet->write ( "B1" , "# Searches" , $format_header );
				$worksheet->write ( "C1" , "# Clicks" , $format_header );
				$worksheet->write ( "D1" , "Click $ (Gross)" , $format_header );
				$worksheet->write ( "E1" , "Click $ (Net)" , $format_header );
				$worksheet->write ( "F1" , "Avg PPC (Gross)" , $format_header );
				$worksheet->write ( "G1" , "Avg PPC (Net)" , $format_header );
				$worksheet->write ( "H1" , "Avg PPS (Gross)" , $format_header );
				$worksheet->write ( "I1" , "Avg PPS (Net)" , $format_header );
				$row = 1;
				foreach ( $searchdata as $searchrecord ) {
						$search_t_click_count += $searchrecord[ click_count ];
						$search_t_search_count += $searchrecord[ num_searches ];
						$search_t_click_amount_gross += $searchrecord[ click_amount_gross ];
						$search_t_click_amount_net += $searchrecord[ click_amount_net ];
						$search_t_fraud_count += $searchrecord[ fraud_count ];
						$ppc_gross = ( $searchrecord[ click_amount_gross ] / $searchrecord[ click_count ] );
						$ppc_net   = ( $searchrecord[ click_amount_net ] / $searchrecord[ click_count ] );
						$pps_gross = ( $searchrecord[ click_amount_gross ] / $searchrecord[ num_searches ] );
						$pps_net   = ( $searchrecord[ click_amount_net ] / $searchrecord[ num_searches ] );
						$worksheet->write ( $row , 0 , $searchrecord[ criteria ] );
						$worksheet->write ( $row , 1 , $searchrecord[ num_searches ] , $count_format );
						$worksheet->write ( $row , 2 , $searchrecord[ click_count ] , $count_format );
						$worksheet->write ( $row , 3 , $searchrecord[ click_amount_gross ] , $dollar_format );
						$worksheet->write ( $row , 4 , $searchrecord[ click_amount_net ] , $dollar_format );
						$worksheet->write ( $row , 5 , $ppc_gross , $dollar_format );
						$worksheet->write ( $row , 6 , $ppc_net , $dollar_format );
						$worksheet->write ( $row , 7 , $pps_gross , $dollar_format );
						$worksheet->write ( $row , 8 , $pps_net , $dollar_format );
						$row++;
				}
				$t_ppc_gross = ( $search_t_click_amount_gross / $search_t_click_count );
				$t_ppc_net   = ( $search_t_click_amount_net / $search_t_click_count );
				$t_pps_gross = ( $search_t_click_amount_gross / $search_t_search_count );
				$t_pps_net   = ( $search_t_click_amount_net / $search_t_search_count );
				$worksheet->write ( $row , 1 , $search_t_search_count , $bold_count_format );
				$worksheet->write ( $row , 2 , $search_t_click_count , $bold_count_format );
				$worksheet->write ( $row , 3 , $search_t_click_amount_gross , $bold_dollar_format );
				$worksheet->write ( $row , 4 , $search_t_click_amount_net , $bold_dollar_format );
				$worksheet->write ( $row , 5 , $t_ppc_gross , $bold_dollar_format );
				$worksheet->write ( $row , 6 , $t_ppc_net , $bold_dollar_format );
				$worksheet->write ( $row , 7 , $t_pps_gross , $bold_dollar_format );
				$worksheet->write ( $row , 8 , $t_pps_net , $bold_dollar_format );
				// CATEGORY Summary
				# Add a worksheet
				$worksheet = & $workbook->addworksheet ( "Category Summary" );
				# Create Headers
				$worksheet->write ( "A1" , "Search Term" , $format_header );
				$worksheet->write ( "B1" , "# Searches" , $format_header );
				$worksheet->write ( "C1" , "# Clicks" , $format_header );
				$worksheet->write ( "D1" , "Click $ (Gross)" , $format_header );
				$worksheet->write ( "E1" , "Click $ (Net)" , $format_header );
				$worksheet->write ( "F1" , "Avg PPC (Gross)" , $format_header );
				$worksheet->write ( "G1" , "Avg PPC (Net)" , $format_header );
				$worksheet->write ( "H1" , "Avg PPS (Gross)" , $format_header );
				$worksheet->write ( "I1" , "Avg PPS (Net)" , $format_header );
				$row = 1;
				foreach ( $categorydata as $categoryrecord ) {
						$cat_t_click_count += $categoryrecord[ click_count ];
						$cat_t_search_count += $categoryrecord[ num_searches ];
						$cat_t_click_amount_gross += $categoryrecord[ click_amount_gross ];
						$cat_t_click_amount_net += $categoryrecord[ click_amount_net ];
						$cat_t_fraud_count += $categoryrecord[ fraud_count ];
						$ppc_gross = ( $categoryrecord[ click_amount_gross ] / $categoryrecord[ click_count ] );
						$ppc_net   = ( $categoryrecord[ click_amount_net ] / $categoryrecord[ click_count ] );
						$pps_gross = ( $categoryrecord[ click_amount_gross ] / $categoryrecord[ num_searches ] );
						$pps_net   = ( $categoryrecord[ click_amount_net ] / $categoryrecord[ num_searches ] );
						$worksheet->write ( $row , 0 , $categoryrecord[ criteria ] );
						$worksheet->write ( $row , 1 , $categoryrecord[ num_searches ] , $count_format );
						$worksheet->write ( $row , 2 , $categoryrecord[ click_count ] , $count_format );
						$worksheet->write ( $row , 3 , $categoryrecord[ click_amount_gross ] , $dollar_format );
						$worksheet->write ( $row , 4 , $categoryrecord[ click_amount_net ] , $dollar_format );
						$worksheet->write ( $row , 5 , $ppc_gross , $dollar_format );
						$worksheet->write ( $row , 6 , $ppc_net , $dollar_format );
						$worksheet->write ( $row , 7 , $pps_gross , $dollar_format );
						$worksheet->write ( $row , 8 , $pps_net , $dollar_format );
						$row++;
				}
				$t_ppc_gross = ( $cat_t_click_amount_gross / $cat_t_click_count );
				$t_ppc_net   = ( $cat_t_click_amount_net / $cat_t_click_count );
				$t_pps_gross = ( $cat_t_click_amount_gross / $cat_t_search_count );
				$t_pps_net   = ( $cat_t_click_amount_net / $cat_t_search_count );
				$worksheet->write ( $row , 1 , $cat_t_search_count , $bold_count_format );
				$worksheet->write ( $row , 2 , $cat_t_click_count , $bold_count_format );
				$worksheet->write ( $row , 3 , $cat_t_click_amount_gross , $bold_dollar_format );
				$worksheet->write ( $row , 4 , $cat_t_click_amount_net , $bold_dollar_format );
				$worksheet->write ( $row , 5 , $t_ppc_gross , $bold_dollar_format );
				$worksheet->write ( $row , 6 , $t_ppc_net , $bold_dollar_format );
				$worksheet->write ( $row , 7 , $t_pps_gross , $bold_dollar_format );
				$worksheet->write ( $row , 8 , $t_pps_net , $bold_dollar_format );
				// BROWSER Summary
				# Add a worksheet
				$worksheet = & $workbook->addworksheet ( "Browser Summary" );
				# Create Headers
				$worksheet->write ( "A1" , "Browser" , $format_header );
				$worksheet->write ( "B1" , "# Clicks" , $format_header );
				$row = 1;
				foreach ( $browserdata as $browserrecord ) {
						$worksheet->write ( $row , 0 , $browserrecord[ criteria ] );
						$worksheet->write ( $row , 1 , $browserrecord[ cnt ] , $count_format );
						$row++;
				}
				// IP Summary
				# Add a worksheet
				$worksheet = & $workbook->addworksheet ( "IP Summary" );
				# Create Headers
				$worksheet->write ( "A1" , "IP Address" , $format_header );
				$worksheet->write ( "B1" , "# Clicks" , $format_header );
				$row = 1;
				foreach ( $ipdata as $iprecord ) {
						$worksheet->write ( $row , 0 , $iprecord[ criteria ] );
						$worksheet->write ( $row , 1 , $iprecord[ cnt ] , $count_format );
						$row++;
				}
				// SEND IT OUT //
				$workbook->close ();
				$xls = implode ( '' , file ( $fname ) );
				ob_end_clean ();
				header ( "Content-Type: application/vnd.ms-excel" );
				header ( "Content-Disposition: attachment; filename=$savename" );
				$xlfh = fopen ( $fname , "rb" );
				fpassthru ( $xlfh );
				exit;
		}

		function financial_summary ()
		{
				global $SCRIPT_NAME;
				global $language;
				global $oSQL;
				global $CGI;
				global $config;
				$end_day      = $CGI[ end_day ];
				$end_month    = $CGI[ end_month ];
				$end_year     = $CGI[ end_year ];
				$start_day    = $CGI[ start_day ];
				$start_month  = $CGI[ start_month ];
				$start_year   = $CGI[ start_year ];
				$downloadable = $CGI[ downloadable ];
				$account      = $CGI[ acct_id ];
				$affiliate    = $CGI[ affiliate_id ];
				$source       = $CGI[ source ];
				$starttime    = $start_year . $start_month . $start_day;
				$endtime      = $end_year . $end_month . $end_day;
				// Table Vars
				$source_summary = SOURCE_SUMMARY_TABLE;
				$aff_summary    = AFF_SUMMARY_TABLE;
				$srcSQL         = "
          SELECT
             source,
             sum(click_count) AS click_count,
             sum(fraud_count) AS fraud_count,
             sum(click_amount_gross) AS click_amount_gross,
             sum(click_amount_net) AS click_amount_net
          FROM $source_summary
          WHERE summary_date BETWEEN $starttime AND $endtime
          GROUP BY source
          ORDER BY source
       ";
				$source_data    = $oSQL->search ( array ( "SQL" => $srcSQL ) );
				$affSQL         = "
          SELECT
             acct_id,
             sum(click_count) AS click_count,
             sum(fraud_count) AS fraud_count,
             sum(click_amount_gross) AS click_amount_gross,
             sum(click_amount_net) AS click_amount_net,
             sum(search_count) AS search_count
          FROM $aff_summary
          WHERE summary_date BETWEEN $starttime AND $endtime
          GROUP BY acct_id
          ORDER BY acct_id
       ";
				$aff_data       = $oSQL->search ( array ( "SQL" => $affSQL ) );
				## Need 2 table formats, for normal PPC ops and for Monthly listings
				$REPORT       = '';
				$SOURCE_TABLE = '';
				$AFF_TABLE    = '';
				$CSV          = '';
				$dates        = array ();
				$affiliates   = array ();
				$sources      = array ();
				$total_clicks = 0;
				$total_gross  = 0;
				$total_net    = 0;
				$total_aff    = 0;
				foreach ( $source_data as $source ) {
						$ppc_gross = 0.00;
						$ppc_net   = 0.00;
						if ( $source{click_count} ) {
								$ppc_gross = sprintf ( "%01.4f" , $source[ click_amount_gross ] / $source[ click_count ] );
								$ppc_net   = sprintf ( "%01.4f" , $source[ click_amount_net ] / $source[ click_count ] );
						}
						$SOURCE_TABLE .= "
               <tr>
                  <td>
                     <b>$source[source]</b>
                  </td>
                  <td align='right'>
                     $source[fraud_count]
                  </td>
                  <td align='right'>
                     $source[click_count]
                  </td>
                  <td align='right'>
                     $source[click_amount_net]
                  </td>
                  <td align='right'>
                     $ppc_gross
                  </td>
               </tr>
           ";
				}
				foreach ( $aff_data as $affiliate ) {
						## Maintain Global Summary ##
						$total_clicks += $affiliate[ click_count ];
						$total_gross += $affiliate[ click_amount_gross ];
						$total_aff += $affiliate[ click_amount_net ];
						$ppc_gross = 0.00;
						$ppc_net   = 0.00;
						if ( $affiliate{click_count} ) {
								$ppc_gross = sprintf ( "%01.4f" , $affiliate[ click_amount_gross ] / $affiliate[ click_count ] );
								$ppc_net   = sprintf ( "%01.4f" , $affiliate[ click_amount_net ] / $affiliate[ click_count ] );
						}
						$AFF_TABLE .= "
               <tr>
                  <td>
                     <b>$affiliate[acct_id]</b>
                  </td>
                  <td align='right'>
                     $affiliate[search_count]
                  </td>
                  <td align='right'>
                     $affiliate[fraud_count]
                  </td>
                  <td align='right'>
                     $affiliate[click_count]
                  </td>
                  <td align='right'>
                     $affiliate[click_amount_gross]
                  </td>
                  <td align='right'>
                     $ppc_gross
                  </td>
                  <td align='right'>
                     $affiliate[click_amount_net]</b>
                  </td>
                  <td align='right'>
                     $ppc_net
                  </td>
               </tr>
           ";
				}
				$report_form = jackhammer_report_form ();
				$total_gross = sprintf ( "%01.4f" , $total_gross );
				$total_aff   = sprintf ( "%01.4f" , $total_aff );
				$gross_ppc   = "0.00";
				$net_ppc     = "0.00";
				if ( $total_clicks ) {
						$gross_ppc = sprintf ( "%01.4f" , $total_gross / $total_clicks );
						$net_ppc   = sprintf ( "%01.4f" , $total_aff / $total_clicks );
				}
				$net = sprintf ( "%01.4f" , $total_gross - $total_aff );

				return "

           <table align='center' cellspacing='10'>
             <tr>
               <td valign='top' style='border-right:1px solid black'>
                  <b>$language[report_summary_header]</b><br />
                  <b>$language[date_header]</b> <i>$start_month/$start_day/$start_year - $end_month/$end_day/$end_year</i><br />
                  <b>$language[breakdown_prompt]</b> <i>$CGI[breakdown]</i><br />
                  <b>$language[acct_id_prompt]</b> <i>$account</i><br />
                  <b>$language[affiliate_prompt]</b> <i>$CGI[affiliate]</i><br />
                  <b>$language[partner_prompt]</b> <i>$CGI[source]</i><br />
               </td>
               <td valign='top'>
                  $report_form
               </td>
             </tr>
             <tr>
               <td style='border-bottom:1px solid black' colspan='2'></td>
             </tr>
           </table>

           <br /><br /><br />

           <b>Report Summary</b><br />
           <table cellspacing='10'>
              <tr>
                 <td valign='top'>
                    <b>$language[total_clicks]</b>: $total_clicks<br />
                    <b>$language[gross_revenue]</b>: $language[dollar_sign]$total_gross<br />
                    <b>$language[aff_payment_report_due]</b>: $language[dollar_sign]$total_aff
                 </td>
                 <td valign='top'>
                    <b>$language[gross_cpc]</b>: $language[dollar_sign]$gross_ppc<br />
                    <b>$language[net_cpc]</b>: $language[dollar_sign]$net_ppc<br />
                    <br />
                    <b>$language[net]: </b> $language[dollar_sign]$net
                 </td>
              </tr>
           </table>

           <br /><br /><br />

           <link rel='stylesheet' type='text/css' href='$config[script_dir]/tablesort.css'>
           <script language='javascript' src='$config[script_dir]/tablesort.js'></script>

           <b>Search Partner Summary</b><br />
           <table cellspacing='0' onclick='sortColumn(event)' class='sorted'>
               <thead>
               <tr>
                  <td>$language[partner_prompt]</td>
                  <td type='Number'>$language[fraud_count]</td>
                  <td type='Number'>$language[total_clicks]</td>
                  <td type='Number'>$language[gross_revenue]</td>
                  <td type='Number'>$language[gross_cpc]</td>
               </tr>
               </thead>

               <tbody>

               $SOURCE_TABLE

               </tbody>

           </table>

           <br /><br /><br />


           <b>Affiliate Summary</b><br />
           <table cellspacing='0' onclick='sortColumn(event)' class='sorted'>
               <thead>
               <tr>
                  <td>$language[affiliate_prompt]</td>
                  <td type='Number'>$language[total_searches]</td>
                  <td type='Number'>$language[fraud_count]</td>
                  <td type='Number'>$language[total_clicks]</td>
                  <td type='Number'>$language[gross_revenue]</td>
                  <td type='Number'>$language[gross_cpc]</td>
                  <td type='Number'>$language[aff_payment_report_due]</td>
                  <td type='Number'>$language[net_cpc]</td>
               </tr>
               </thead>

               <tbody>

               $AFF_TABLE

               </tbody>

           </table>

       ";
		}

		function click_report ()
		{
				global $SCRIPT_NAME;
				global $language;
				global $oSQL;
				global $CGI;
				global $config;
				$end_day      = $CGI[ end_day ];
				$end_month    = $CGI[ end_month ];
				$end_year     = $CGI[ end_year ];
				$start_day    = $CGI[ start_day ];
				$start_month  = $CGI[ start_month ];
				$start_year   = $CGI[ start_year ];
				$downloadable = $CGI[ downloadable ];
				$account      = $CGI[ acct_id ];
				$affiliate    = $CGI[ affiliate_id ];
				$source       = $CGI[ source ];
				// Table Vars
				$source_summary = SOURCE_SUMMARY_TABLE;
				$aff_summary    = AFF_SUMMARY_TABLE;
				$daily_summary  = DAILY_SUMMARY_TABLE;
				$click_summary  = CLICK_SUMMARY_TABLE;
				$click_log      = CLICK_LOG_TABLE;
				## Pre-Populate the query string for detail links, with known info
				$query_string = "admin=hyperseek&action=report&report_type=click_detail&breakdown=$CGI[breakdown]&acct_id=$account&affiliate_id=$affiliate&source=$source";
				$starttime    = $start_year . $start_month . $start_day;
				$endtime      = $end_year . $end_month . $end_day;
				## Build up the SQL ##
				$SQL          = '';
				$WHERE        = '';
				$num_selected = 0;
				if ( $affiliate ) {
						$num_selected++;
				}
				if ( $source ) {
						$num_selected++;
				}
				if ( $account ) {
						$num_selected++;
				}
				## Any combination, we have to use the big nasty table and be slow ##
				if ( ( $affiliate && $CGI[ breakdown ] && $CGI[ breakdown ] == "source" ) || ( $source && $CGI[ breakdown ] && $CGI[ breakdown ] == "affiliate" ) || ( $num_selected > 1 )
				) {
						if ( $account ) {
								$WHERE .= " AND account = '$account' ";
						}
						if ( $affiliate ) {
								$WHERE .= " AND affiliate = '$affiliate' ";
						}
						if ( $source ) {
								$WHERE .= " AND ppc_engine = '$source' ";
						}
						$SQL = "
              SELECT
                hit_date, ppc_engine, affiliate,
                sum(price),
                sum(gross_price) AS price,
                ppc_commission,
                commission,
                paid_flag as paid_status,
                sum(price) as net_price,
                count(hit_id) AS num_clicks
              FROM $click_log
              WHERE hit_date BETWEEN $starttime AND $endtime
              $WHERE
              group by hit_date
              ORDER BY hit_date
           ";
				} ## Account Only
				elseif ( $account && ( !$affiliate && !$source ) ) {
						$SQL = "
              SELECT
                acct_id,
                click_date AS hit_date,
                click_amount_gross AS price,
                click_amount_net AS net_price,
                click_count AS num_clicks,
                fraud_count AS num_fraud
              FROM $click_summary
              WHERE  acct_id = '$account'
              AND click_date BETWEEN $starttime AND $endtime
              ORDER BY click_date
           ";
				} ## Affiliate Only
				elseif ( $affiliate || $CGI[ breakdown ] == "affiliate" ) {
						$lookup = '';
						if ( $affiliate ) {
								$lookup = "acct_id = '$affiliate' AND";
						}
						$SQL = "
              SELECT
                summary_date AS hit_date,
                click_amount_gross AS price,
                click_amount_net AS net_price,
                interim_click_count AS num_interim_clicks,
                click_count AS num_clicks,
                search_count AS num_searches,
                fraud_count AS num_fraud,
                acct_id AS affiliate
              FROM $aff_summary
              WHERE $lookup summary_date BETWEEN $starttime AND $endtime
              ORDER BY summary_date
           ";
				} ## Source Only
				elseif ( $source || $CGI[ breakdown ] == "source" ) {
						$lookup = '';
						if ( $source ) {
								$lookup = "source = '$source' AND";
						}
						$SQL = "
              SELECT
                summary_date AS hit_date,
                click_amount_gross AS price,
                click_amount_net AS net_price,
                click_count AS num_clicks,
                fraud_count AS num_fraud,
                source AS ppc_engine
              FROM $source_summary
              WHERE $lookup summary_date BETWEEN $starttime AND $endtime
              ORDER BY summary_date
           ";
				} ## No criteria
				elseif ( !$source && !$account && !$affiliate ) {
						$SQL = "
              SELECT
                summary_date AS hit_date,
                click_amount_gross AS price,
                click_amount_net AS net_price,
                click_count AS num_clicks,
                search_count AS num_searches,
                fraud_count AS num_fraud
              FROM $daily_summary
              WHERE summary_date BETWEEN $starttime AND $endtime
              ORDER BY summary_date
           ";
				}
				$report_data = $oSQL->search ( array ( "SQL" => $SQL ) );
				$REPORT      = '';
				$TABLE       = '';
				$CSV         = '';
				$dates       = array ();
				$affiliates  = array ();
				$sources     = array ();
				foreach ( $report_data as $hit ) {
						$date               = $hit[ hit_date ];
						$engine             = $hit[ ppc_engine ];
						$affiliate_id       = $hit[ affiliate ];
						$gross_price        = $hit[ price ];
						$price              = $hit[ price ];
						$ppc_commission     = $hit[ ppc_commisssion ];
						$aff_commission     = $hit[ commission ];
						$paid_status        = $hit[ paid_status ];
						$num_clicks         = $hit[ num_clicks ];
						$num_searches       = $hit[ num_searches ];
						$num_fraud          = $hit[ num_fraud ];
						$aff_net            = $hit[ net_price ];
						$num_interim_clicks = $hit[ num_interim_clicks ];
						$aff_earnings       = 0.00;
						if ( $affiliate_id && $price > $aff_net ) {
								$aff_earnings = $aff_net;
						} else {
								$aff_earnings = $gross_price - $aff_net;
						}
						$dates[ $date ][ gross_total ] += $price;
						$dates[ $date ][ affiliate_total ] += $aff_earnings;
						$dates[ $date ][ fraud_count ] += $num_fraud;
						$dates[ $date ][ search_count ] += $num_searches;
						$affiliates[ $date ][ $affiliate_id ][ gross_total ] += $price;
						$affiliates[ $date ][ $affiliate_id ][ affiliate_total ] += $aff_earnings;
						$affiliates[ $date ][ $affiliate_id ][ fraud_count ] += $num_fraud;
						$affiliates[ $date ][ $affiliate_id ][ search_count ] += $num_searches;
						$affiliates[ $date ][ $affiliate_id ][ num_interim_clicks ] += $num_interim_clicks;
						$sources[ $date ][ $engine ][ gross_total ] += $price;
						$sources[ $date ][ $engine ][ fraud_count ] += $num_fraud;
						$sources[ $date ][ $engine ][ revenue_total ] += $price;
						$sources[ $date ][ $engine ][ affiliate_total ] += $aff_earnings;
						$sources[ $date ][ $engine ][ search_count ] = "n/a";
						if ( $num_clicks ) {
								$dates[ $date ][ click_count ] += $num_clicks;
								$affiliates[ $date ][ $affiliate_id ][ click_count ] += $num_clicks;
								$sources[ $date ][ $engine ][ click_count ] += $num_clicks;
						}
						/* else {
												$dates[$date][click_count] = $num_clicks;
												$affiliates[$date][$affiliate_id][click_count] = $num_clicks;
												$sources[$date][$engine][click_count] = $num_clicks;
							}*/
				}
				$total_clicks     = 0;
				$total_searches   = 0;
				$total_fraud      = 0;
				$total_gross      = 0;
				$total_revenue    = 0;
				$total_aff        = 0;
				$total_net        = 0;
				$breakdown_totals = array ();
				foreach ( $dates as $date => $vals ) {
						$show_date                         = formatdate ( $date );
						$dates[ $date ][ gross_total ]     = sprintf ( "%01.4f" , $dates[ $date ][ gross_total ] );
						$dates[ $date ][ revenue_total ]   = sprintf ( "%01.4f" , $dates[ $date ][ revenue_total ] );
						$dates[ $date ][ affiliate_total ] = sprintf ( "%01.4f" , $dates[ $date ][ affiliate_total ] );
						$date_net                          = sprintf ( "%01.4f" , $dates[ $date ][ gross_total ] - $dates[ $date ][ affiliate_total ] );
						$total_clicks += $dates[ $date ][ click_count ];
						$total_searches += $dates[ $date ][ search_count ];
						$total_fraud += $dates[ $date ][ fraud_count ];
						$total_gross += $dates[ $date ][ gross_total ];
						$total_revenue += $dates[ $date ][ revenue_total ];
						$total_aff += $dates[ $date ][ affiliate_total ];
						$total_net += $date_net;
						$d       = preg_split ( "//" , $date );
						$mon     = $d[ 5 ] . $d[ 6 ];
						$day     = $d[ 7 ] . $d[ 8 ];
						$year    = $d[ 1 ] . $d[ 2 ] . $d[ 3 ] . $d[ 4 ];
						$date_qs = "&start_month=$mon&start_day=$day&start_year=$year&end_month=$mon&end_day=$day&end_year=$year";
						$link    = $query_string . "&" . $date_qs;
						if ( $CGI[ breakdown ] ) {
								$breakdown = array ();
								if ( $CGI[ breakdown ] == "affiliate" ) {
										$breakdown = $affiliates;
								}
								if ( $CGI[ breakdown ] == "source" ) {
										$breakdown = $sources;
								}
								foreach ( $breakdown[ $date ] as $id => $idvals ) {
										$breakdown[ $date ][ $id ][ gross_total ]     = sprintf ( "%01.4f" , $breakdown[ $date ][ $id ][ gross_total ] );
										$breakdown[ $date ][ $id ][ revenue_total ]   = sprintf ( "%01.4f" , $breakdown[ $date ][ $id ][ revenue_total ] );
										$breakdown[ $date ][ $id ][ affiliate_total ] = sprintf ( "%01.4f" , $breakdown[ $date ][ $id ][ affiliate_total ] );
										$bd_net                                       = sprintf ( "%01.4f" , $breakdown[ $date ][ $id ][ gross_total ] - $breakdown[ $date ][ $id ][ affiliate_total ] );
										$interim                                      = '';
										if ( $breakdown[ $date ][ $id ][ num_interim_clicks ] ) {
												$interim = $breakdown[ $date ][ $id ][ num_interim_clicks ];
												$total_interim += $breakdown[ $date ][ $id ][ num_interim_clicks ];
										}
										## Add source or affiliate id to the link ##
										$thislink = $SCRIPT_NAME . "?" . $query_string . "&detail=$id&" . "&start_month=$start_month&start_day=$start_day&start_year=$start_year&end_month=$end_month&end_day=$end_day&end_year=$end_year";
										// Total Vars
										$sc                               = $breakdown[ $date ][ $id ][ search_count ];
										$fc                               = $breakdown[ $date ][ $id ][ fraud_count ];
										$cc                               = $breakdown[ $date ][ $id ][ click_count ];
										$gt                               = $breakdown[ $date ][ $id ][ gross_total ];
										$at                               = $breakdown[ $date ][ $id ][ affiliate_total ];
										$net                              = $bd_net;
										$breakdown_totals[ $id ][ dlink ] = $thislink;
										$breakdown_totals[ $id ][ search_count ] += $sc;
										$breakdown_totals[ $id ][ fraud_count ] += $fc;
										$breakdown_totals[ $id ][ click_count ] += $cc;
										$breakdown_totals[ $id ][ gross_total ] += $gt;
										$breakdown_totals[ $id ][ affiliate_total ] += $at;
										$breakdown_totals[ $id ][ interim_clicks ] += $interim;
										$breakdown_totals[ $id ][ net ] += $net;
								}
						} else {
								// Total Vars
								$sc = $dates[ $date ][ search_count ];
								$fc = $dates[ $date ][ fraud_count ];
								$cc = $dates[ $date ][ click_count ];
								$gt = $dates[ $date ][ gross_total ];
								$at = $dates[ $date ][ affiliate_total ];
								$TABLE .= "
                  <tr>
                      <td align='left'>
                         <a class='rptlink' href='$SCRIPT_NAME?$link'>$show_date</a>
                      </td>

                      <td align='right'>
                         $sc
                      </td>

                      <td align='right'>
                         $fc
                      </td>

                      <td align='right'>
                         $cc
                      </td>

                      <td align='right'>
                         $language[dollar_sign] $gt
                      </td>

                      <td align='right'>
                         $language[dollar_sign] $at
                      </td>

                      <td align='right'>
                         $language[dollar_sign] $date_net
                      </td>

                  </tr>
               ";
						}
				}
				if ( $CGI[ breakdown ] ) {
						foreach ( $breakdown_totals as $id => $stats ) {
								if ( $stats[ interim_clicks ] ) {
										$iclicks = "
                      <td align='right'>
                           $stats[interim_clicks]
                      </td>
                   ";
								}
								$TABLE .= "
                  <tr>
                      <td align='left'>
                         <a class='rptlink' href='$stats[dlink]'>$id</a>
                      </td>

                      <td align='right'>
                         $stats[search_count]
                      </td>

                      <td align='right'>
                         $stats[fraud_count]
                      </td>

                      $iclicks

                      <td align='right'>
                           $stats[click_count]
                      </td>

                      <td align='right'>
                         $stats[gross_total]
                      </td>

                      <td align='right'>
                         $stats[affiliate_total]
                      </td>

                      <td align='right'>
                         $stats[net]
                      </td>
                  </tr>
               ";
						}
				}
				$report_form   = jackhammer_report_form ();
				$total_gross   = sprintf ( "%01.4f" , $total_gross );
				$total_revenue = sprintf ( "%01.4f" , $total_revenue );
				$total_aff     = sprintf ( "%01.4f" , $total_aff );
				$total_net     = sprintf ( "%01.4f" , $total_net );
				if ( $CGI[ breakdown ] ) {
						$language[ date_header ] = $CGI[ breakdown ];
				}
				if ( $iclicks ) {
						$iclicks_header = "
              <td>Interim Clicks</td>
           ";
						$iclicks_total  = "
                  <td align='right' style='border-top:1px solid black'>
                     $total_interim
                  </td>
           ";
				}

				return "
           <table align='center' cellspacing='10'>
             <tr>
               <td valign='top' style='border-right:1px solid black'>
                  <b>$language[report_summary_header]</b><br />
                  <b>$language[date_header]</b> <i>$start_month/$start_day/$start_year - $end_month/$end_day/$end_year</i><br />
                  <b>$language[breakdown_prompt]</b> <i>$CGI[breakdown]</i><br />
                  <b>$language[acct_id_prompt]</b> <i>$account</i><br />
                  <b>$language[affiliate_prompt]</b> <i>$affiliate</i><br />
                  <b>$language[partner_prompt]</b> <i>$source</i><br />
               </td>
               <td valign='top'>
                  $report_form
               </td>
             </tr>
             <tr>
               <td style='border-bottom:1px solid black' colspan='2'></td>
             </tr>
           </table>

           <link rel='stylesheet' href='$config[script_dir]/tablesort.css'>
           <script language='javascript' src='$config[script_dir]/tablesort.js'></script>
           <table align='center' class='sorted' onClick='sortColumn(event)'>
               <thead>
               <tr>
                  <td>$language[date_header]</td>
                  <td>$language[total_searches]</td>
                  <td>$language[fraud_count]</td>
                  $iclicks_header
                  <td>$language[total_clicks]</td>
                  <td>$language[gross_revenue]</td>
                  <td>$language[aff_payment_report_due]</td>
                  <td>$language[net]</td>
               </tr>
               </thead>

               <tbody>

               $TABLE

               </tbody>

               <tr>
                  <td align='right' style='border-top:1px solid black'>
                     <b>$language[report_total]</b>
                  </td>

                  <td align='right' style='border-top:1px solid black'>
                     $total_searches
                  </td>

                  <td align='right' style='border-top:1px solid black'>
                     $total_fraud
                  </td>

                  $iclicks_total

                  <td align='right' style='border-top:1px solid black'>
                     $total_clicks
                  </td>

                  <td align='right' style='border-top:1px solid black'>
                     $language[dollar_sign]$total_gross
                  </td>

                  <td align='right' style='border-top:1px solid black'>
                     $language[dollar_sign]$total_aff
                  </td>

                  <td align='right' style='border-top:1px solid black'>
                     $language[dollar_sign]$total_net
                  </td>

               </tr>


           </table>
       ";
		}


		function click_report_detail ()
		{
				global $SCRIPT_NAME;
				global $language;
				global $oSQL;
				global $CGI;
				global $config;
				$end_day      = $CGI[ end_day ];
				$end_month    = $CGI[ end_month ];
				$end_year     = $CGI[ end_year ];
				$start_day    = $CGI[ start_day ];
				$start_month  = $CGI[ start_month ];
				$start_year   = $CGI[ start_year ];
				$downloadable = $CGI[ downloadable ];
				// Table Vars
				$click_log = CLICK_LOG_TABLE;
				## Force a minimal lookup, if sent in a detail id
				if ( $CGI[ breakdown ] == "source" && !$CGI[ source ] ) {
						$CGI[ source ] = $CGI[ detail ];
				}
				if ( $CGI[ breakdown ] == "affiliate" && !$CGI[ affiliate_id ] ) {
						$CGI[ affiliate_id ] = $CGI[ detail ];
				}
				$account   = $CGI[ acct_id ];
				$affiliate = $CGI[ affiliate_id ];
				$source    = $CGI[ source ];
				## Pre-Populate the query string for detail links, with known info
				$query_string = "admin=hyperseek&action=report&report_type=keyword_detail&breakdown=$CGI[breakdown]&acct_id=$account&affiliate_id=$affiliate&source=$source";
				$date_qs      = "admin=hyperseek&&start_month=$start_month&start_day=$start_day&start_year=$start_year&end_month=$end_month&end_day=$end_day&end_year=$end_year";
				$starttime    = $start_year . $start_month . $start_day;
				$endtime      = $end_year . $end_month . $end_day;
				## Build up the SQL ##
				$WHERE = '';
				if ( $account ) {
						$WHERE .= " AND account = '$account' ";
				}
				if ( $affiliate ) {
						$WHERE .= " AND affiliate = '$affiliate' ";
				}
				if ( $source ) {
						$WHERE .= " AND ppc_engine = '$source' ";
				}
				$SQL         = "
          SELECT * FROM $click_log
          WHERE hit_date BETWEEN $starttime AND $endtime
          $WHERE
          ORDER BY hit_date
       ";
				$report_data = $oSQL->search ( array ( "SQL" => $SQL ) );
				## Need 2 table formats, for normal PPC ops and for Monthly listings
				$REPORT        = '';
				$TABLE         = '';
				$TABLE_MONTHLY = '';
				$CSV           = '';
				$keywords      = array ();
				$affiliates    = array ();
				$sources       = array ();
				foreach ( $report_data as $hit ) {
						$date            = $hit[ hit_date ];
						$engine          = $hit[ ppc_engine ];
						$affiliate_id    = $hit[ affiliate ];
						$gross_price     = $hit[ gross_price ];
						$price           = $hit[ price ];
						$ppc_commission  = $hit[ ppc_commisssion ];
						$aff_commission  = $hit[ commission ];
						$paid_status     = $hit[ paid_status ];
						$keyword         = $hit[ word ];
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
						$keywords[ $keyword ][ $gross_price ][ gross_total ] += $gross_price;
						$keywords[ $keyword ][ $gross_price ][ revenue_total ] += $price;
						$keywords[ $keyword ][ $gross_price ][ affiliate_total ] += $aff_earnings;
						$keywords[ $keyword ][ $gross_price ][ click_count ]++;
						$affiliates[ $keyword ][ $gross_price ][ $affiliate_id ][ gross_total ] += $gross_price;
						$affiliates[ $keyword ][ $gross_price ][ $affiliate_id ][ revenue_total ] += $price;
						$affiliates[ $keyword ][ $gross_price ][ $affiliate_id ][ affiliate_total ] += $aff_earnings;
						$affiliates[ $keyword ][ $gross_price ][ $affiliate_id ][ click_count ]++;
						$sources[ $keyword ][ $gross_price ][ $engine ][ gross_total ] += $gross_price;
						$sources[ $keyword ][ $gross_price ][ $engine ][ revenue_total ] += $price;
						$sources[ $keyword ][ $gross_price ][ $engine ][ affiliate_total ] += $aff_earnings;
						$sources[ $keyword ][ $gross_price ][ $engine ][ click_count ]++;
				}
				$total_clicks  = 0;
				$total_gross   = 0;
				$total_revenue = 0;
				$total_aff     = 0;
				$total_net     = 0;
				foreach ( $keywords as $keyword => $vals ) {
						$keyword_total_clicks  = 0;
						$keyword_total_gross   = 0;
						$keyword_total_revenue = 0;
						$keyword_total_aff     = 0;
						$keyword_total_net     = 0;
						$detail_rows           = '';
						## Break Down Each Price Level
						foreach ( $keywords[ $keyword ] as $bidprice => $bpvals ) {
								## Accumulate Totals for this keyword
								$keywords[ $keyword ][ $bidprice ][ gross_total ]     = sprintf ( "%01.4f" , $keywords[ $keyword ][ $bidprice ][ gross_total ] );
								$keywords[ $keyword ][ $bidprice ][ revenue_total ]   = sprintf ( "%01.4f" , $keywords[ $keyword ][ $bidprice ][ revenue_total ] );
								$keywords[ $keyword ][ $bidprice ][ affiliate_total ] = sprintf ( "%01.4f" , $keywords[ $keyword ][ $bidprice ][ affiliate_total ] );
								$keyword_total_clicks += $keywords[ $keyword ][ $bidprice ][ click_count ];
								$keyword_total_gross += $keywords[ $keyword ][ $bidprice ][ gross_total ];
								$keyword_total_revenue += $keywords[ $keyword ][ $bidprice ][ revenue_total ];
								$keyword_total_aff += $keywords[ $keyword ][ $bidprice ][ affiliate_total ];
								$keyword_total_net = $keyword_total_revenue - $keyword_total_aff;
								## Accumulate Master Totals
								$total_clicks += $keywords[ $keyword ][ $bidprice ][ click_count ];
								$total_gross += $keywords[ $keyword ][ $bidprice ][ gross_total ];
								$total_revenue += $keywords[ $keyword ][ $bidprice ][ revenue_total ];
								$total_aff += $keywords[ $keyword ][ $bidprice ][ affiliate_total ];
								//$total_net     = $keyword_total_revenue - $keyword_total_aff;
								$total_net += $keyword_total_revenue - $keyword_total_aff; // ADDed + for total by AKhilesh on 5/4/2011
								$showprice                                            = sprintf ( "%01.4f" , $bidprice );
								$price_net                                            = sprintf ( "%01.4f" , $keywords[ $keyword ][ $bidprice ][ revenue_total ] - $keywords[ $keyword ][ $bidprice ][ affiliate_total ] );
								$keywords[ $keyword ][ $bidprice ][ gross_total ]     = sprintf ( "%01.4f" , $keywords[ $keyword ][ $bidprice ][ gross_total ] );
								$keywords[ $keyword ][ $bidprice ][ revenue_total ]   = sprintf ( "%01.4f" , $keywords[ $keyword ][ $bidprice ][ revenue_total ] );
								$keywords[ $keyword ][ $bidprice ][ affiliate_total ] = sprintf ( "%01.4f" , $keywords[ $keyword ][ $bidprice ][ affiliate_total ] );
								// Printable Vars
								$cc = $keywords[ $keyword ][ $bidprice ][ click_count ];
								$gt = $keywords[ $keyword ][ $bidprice ][ gross_total ];
								$rt = $keywords[ $keyword ][ $bidprice ][ revenue_total ];
								$at = $keywords[ $keyword ][ $bidprice ][ affiliate_total ];
								$detail_rows .= "

                  <tr>
                      <td>&nbsp;</td>
                      <td align='right'>
                         $language[dollar_sign]<b>$showprice</b>
                      </td>

                      <td align='right'>
                         $cc
                      </td>

                      <td align='right'>
                         $language[dollar_sign] $gt
                      </td>

                      <td align='right'>
                         $language[dollar_sign] $rt
                      </td>

                      <td align='right'>
                         $language[dollar_sign] $at
                      </td>

                      <td align='right'>
                         $language[dollar_sign] $price_net
                      </td>

                  </tr>
               ";
						}
						$encoded_key           = urlencode ( $keyword );
						$link                  = $query_string . "&" . $date_qs . "&keyword=$encoded_key";
						$keyword_total_gross   = sprintf ( "%01.4f" , $keyword_total_gross );
						$keyword_total_revenue = sprintf ( "%01.4f" , $keyword_total_revenue );
						$keyword_total_aff     = sprintf ( "%01.4f" , $keyword_total_aff );
						$keyword_total_net     = sprintf ( "%01.4f" , $keyword_total_net );
						$TABLE .= "
                  <tr>
                     <td align='left' colspan='7'>
                        <a href='$SCRIPT_NAME?$link'><b>$keyword</b></a>
                    </td>
                  </tr>

                  $detail_rows

                  <tr>
                      <td align='right' colspan='2' style='border-top:1px solid silver;border-bottom:1px solid silver'>
                         $language[report_subtotal]
                      </td>

                      <td align='right' style='border-top:1px solid silver;border-bottom:1px solid silver'>
                         $keyword_total_clicks
                      </td>

                      <td align='right' style='border-top:1px solid silver;border-bottom:1px solid silver'>
                         $language[dollar_sign] $keyword_total_gross
                      </td>

                      <td align='right' style='border-top:1px solid silver;border-bottom:1px solid silver'>
                         $language[dollar_sign] $keyword_total_revenue
                      </td>

                      <td align='right' style='border-top:1px solid silver;border-bottom:1px solid silver'>
                         $language[dollar_sign] $keyword_total_aff
                      </td>

                      <td align='right' style='border-top:1px solid silver;border-bottom:1px solid silver'>
                         $language[dollar_sign] $keyword_total_net
                      </td>

                  </tr>

               ";
				}
				$report_form   = jackhammer_report_form ();
				$total_gross   = sprintf ( "%01.4f" , $total_gross );
				$total_revenue = sprintf ( "%01.4f" , $total_revenue );
				$total_aff     = sprintf ( "%01.4f" , $total_aff );
				$total_net     = sprintf ( "%01.4f" , $total_net );

				return "

           <table align='center' cellspacing='10'>
             <tr>
               <td valign='top' style='border-right:1px solid black'>
                  <b>$language[report_summary_header]</b><br />
                  <b>$language[date_header]</b> <i>$start_month/$start_day/$start_year - $end_month/$end_day/$end_year</i><br />
                  <b>$language[breakdown_prompt]</b> <i>$CGI[breakdown]</i><br />
                  <b>$language[acct_id_prompt]</b> <i>$account</i><br />
                  <b>$language[affiliate_prompt]</b> <i>$affiliate</i><br />
                  <b>$language[partner_prompt]</b> <i>$source</i><br />
               </td>
               <td valign='top'>
                  $report_form
               </td>
             </tr>
             <tr>
               <td style='border-bottom:1px solid black' colspan='2'></td>
             </tr>
           </table>
           <link rel='stylesheet' href='$config[script_dir]/tablesort.css'>
           <table align='center' class='sorted'>
               <thead>
               <tr>
                  <td>$language[date_header]</td>
                  <td>$language[price_header]</td>
                  <td>$language[total_clicks]</td>
                  <td>$language[total_revenue]</td>
                  <td>$language[net_revenue]</td>
                  <td>$language[aff_payment_report_due]</td>
                  <td>$language[net]</td>
               </tr>
               </thead>

               <tbody>

               $TABLE

               </tbody>

               <tr>
                  <td colspan='2' align='right' style='border-top:1px solid black'>
                     <b>Report Total</b>
                  </td>

                  <td align='right' style='border-top:1px solid black'>
                     $total_clicks
                  </td>

                  <td align='right' style='border-top:1px solid black'>
                     $language[dollar_sign]$total_gross
                  </td>

                  <td align='right' style='border-top:1px solid black'>
                     $language[dollar_sign]$total_revenue
                  </td>

                  <td align='right' style='border-top:1px solid black'>
                     $language[dollar_sign]$total_aff
                  </td>

                  <td align='right' style='border-top:1px solid black'>
                     $language[dollar_sign]$total_net
                  </td>

               </tr>


           </table>
       ";
		}


		function keyword_report_detail ()
		{
				global $SCRIPT_NAME;
				global $language;
				global $oSQL;
				global $CGI;
				global $config;
				$end_day     = $CGI[ end_day ];
				$end_month   = $CGI[ end_month ];
				$end_year    = $CGI[ end_year ];
				$start_day   = $CGI[ start_day ];
				$start_month = $CGI[ start_month ];
				$start_year  = $CGI[ start_year ];
				// Table Vars
				$click_log    = CLICK_LOG_TABLE;
				$downloadable = $CGI[ downloadable ];
				## Force a minimal lookup, if sent in a detail id
				if ( $CGI[ breakdown ] == "source" && !$CGI[ source ] ) {
						$CGI[ source ] = $CGI[ detail ];
				}
				if ( $CGI[ breakdown ] == "affiliate" && !$CGI[ affiliate_id ] ) {
						$CGI[ affiliate_id ] = $CGI[ detail ];
				}
				$account   = $CGI[ acct_id ];
				$affiliate = $CGI[ affiliate_id ];
				$source    = $CGI[ source ];
				## Pre-Populate the query string for detail links, with known info
				$query_string = "admin=hyperseek&action=report&report_type=keyword_detail&breakdown=$CGI[breakdown]&acct_id=$account&affiliate_id=$affiliate&source=$source";
				$date_qs      = "admin=hyperseek&&start_month=$start_month&start_day=$start_day&start_year=$start_year&end_month=$end_month&end_day=$end_day&end_year=$end_year";
				$starttime    = $start_year . $start_month . $start_day;
				$endtime      = $end_year . $end_month . $end_day;
				## Build up the SQL ##
				$WHERE = '';
				if ( $account ) {
						$WHERE .= " AND account = '$account' ";
				}
				if ( $affiliate ) {
						$WHERE .= " AND affiliate = '$affiliate' ";
				}
				if ( $source ) {
						$WHERE .= " AND ppc_engine = '$source' ";
				}
				$SQL         = "
          SELECT * FROM $click_log
          WHERE hit_date BETWEEN $starttime AND $endtime
          AND word = '$CGI[keyword]'
          $WHERE
          ORDER BY hit_date
       ";
				$report_data = $oSQL->search ( array ( "SQL" => $SQL ) );
				$TABLE       = "";
				foreach ( $report_data as $hit ) {
						$hit[ gross_price ] = sprintf ( "%01.4f" , $hit[ gross_price ] );
						$hit[ price ]       = sprintf ( "%01.4f" , $hit[ price ] );
						$date               = $hit[ hit_date ];
						$date               = substr ( $date , 0 , 6 );
						$TABLE .= "
               <tr>
                  <td><a href='$SCRIPT_NAME?admin=hyperseek&action=report&report_type=view_click&hit_id=$hit[hit_id]&tbl=$date'>$hit[word]</a></td>
                  <td>$hit[ppc_engine]</td>
                  <td>$hit[affiliate]</td>
                  <td>$language[dollar_sign] $hit[gross_price]</td>
                  <td>$language[dollar_sign] $hit[price]</td>
               </tr>
          ";
				}
				$report_form = jackhammer_report_form ();

				return "

           <table align='center' cellspacing='10'>
             <tr>
               <td valign='top' style='border-right:1px solid black'>
                  <b>$language[report_summary_header]</b><br />
                  <b>$language[date_header]</b> <i>$start_month/$start_day/$start_year - $end_month/$end_day/$end_year</i><br />
                  <b>$language[breakdown_prompt]</b> <i>$CGI[breakdown]</i><br />
                  <b>$language[manage_bids_keyword]</b> <i>$CGI[keyword]</i><br />
                  <b>$language[acct_id_prompt]</b> <i>$account</i><br />
                  <b>$language[affiliate_prompt]</b> <i>$affiliate</i><br />
                  <b>$language[partner_prompt]</b> <i>$source</i><br />
               </td>
               <td valign='top'>
                  $report_form
               </td>
             </tr>
             <tr>
               <td style='border-bottom:1px solid black' colspan='2'></td>
             </tr>
           </table>

           <link rel='stylesheet' href='$config[script_dir]/tablesort.css'>
           <table align='center' class='sorted'>
               <thead>
               <tr>
                  <td>$language[manage_bids_keyword]</td>
                  <td>$language[partner_prompt]</td>
                  <td>$language[affiliate_prompt]</td>
                  <td>$language[total_revenue]</td>
                  <td>$language[net_price]</td>
               </tr>
               </thead>

               <tbody>
               $TABLE
               </tbody>
           </table>
       ";
		}


		function fraud_report ()
		{
				global $SCRIPT_NAME;
				global $language;
				global $oSQL;
				global $CGI;
				global $config;
				$end_day      = $CGI[ end_day ];
				$end_month    = $CGI[ end_month ];
				$end_year     = $CGI[ end_year ];
				$start_day    = $CGI[ start_day ];
				$start_month  = $CGI[ start_month ];
				$start_year   = $CGI[ start_year ];
				$downloadable = $CGI[ downloadable ];
				// Table Vars
				$click_fraud = CLICK_FRAUD_TABLE;
				## Force a minimal lookup, if sent in a detail id
				if ( $CGI[ breakdown ] == "source" && !$CGI[ source ] ) {
						$CGI[ source ] = $CGI[ detail ];
				}
				if ( $CGI[ breakdown ] == "affiliate" && !$CGI[ affiliate_id ] ) {
						$CGI[ affiliate_id ] = $CGI[ detail ];
				}
				$account    = $CGI[ acct_id ];
				$affiliate  = $CGI[ affiliate_id ];
				$source     = $CGI[ source ];
				$fraud_code = $CGI[ fraud_code ];
				## Pre-Populate the query string for detail links, with known info
				$query_string = "admin=hyperseek&action=report&report_type=fraud_list&breakdown=$CGI[breakdown]&acct_id=$account&affiliate_id=$affiliate&source=$source";
				$starttime    = $start_year . $start_month . $start_day;
				$endtime      = $end_year . $end_month . $end_day;
				## Build up the SQL ##
				$WHERE = '';
				if ( $account ) {
						$WHERE .= " AND account = '$account' ";
				}
				if ( $affiliate ) {
						$WHERE .= " AND affiliate = '$affiliate' ";
				}
				if ( $source ) {
						$WHERE .= " AND ppc_engine = '$source' ";
				}
				$SQL         = "
          SELECT * FROM $click_fraud
          WHERE hit_date BETWEEN $starttime AND $endtime
          $WHERE
          ORDER BY hit_date
       ";
				$report_data = $oSQL->search ( array ( "SQL" => $SQL ) );
				## Need 2 table formats, for normal PPC ops and for Monthly listings
				$REPORT        = '';
				$TABLE         = '';
				$TABLE_MONTHLY = '';
				$CSV           = '';
				$dates         = array ();
				$affiliates    = array ();
				$sources       = array ();
				foreach ( $report_data as $hit ) {
						$date         = $hit[ hit_date ];
						$engine       = $hit[ ppc_engine ];
						$affiliate_id = $hit[ affiliate ];
						$gross_price  = $hit[ price ];
						$price        = $hit[ price ];
						$fraud_code   = $hit[ fraud_id ];
						$dates[ $date ][ $fraud_code ][ 'count' ]++;
						$affiliates[ $date ][ $fraud_code ][ $affiliate_id ][ 'count' ]++;
						$sources[ $date ][ $fraud_code ][ $engine ][ 'count' ]++;
				}
				$total_fraud = 0;
				foreach ( $dates as $date => $vals ) {
						$show_date = formatdate ( $date );
						$d         = preg_split ( '//' , $date );
						$mon       = $d[ 5 ] . $d[ 6 ];
						$day       = $d[ 7 ] . $d[ 8 ];
						$year      = $d[ 1 ] . $d[ 2 ] . $d[ 3 ] . $d[ 4 ];
						$date_qs   = "&start_month=$mon&start_day=$day&start_year=$year&end_month=$mon&end_day=$day&end_year=$year";
						foreach ( $dates[ $date ] as $fraud_code => $fcvals ) {
								$total_fraud += $dates[ $date ][ $fraud_code ][ 'count' ];
								if ( $CGI[ breakdown ] ) {
										$link = $query_string . "&" . $date_qs . "&fraud_code=$fraud_code";
										$TABLE .= "
                      <tr>
                          <td>$show_date</td>
                          <td align='left'>
                             <b><a href='$SCRIPT_NAME?$link'>$fraud_code</a></b>
                          </td>
                          <td>&nbsp;</td>
                      </tr>
                   ";
										$breakdown = array ();
										if ( $CGI[ breakdown ] == "affiliate" ) {
												$breakdown = $affiliates;
										}
										if ( $CGI[ breakdown ] == "source" ) {
												$breakdown = $sources;
										}
										foreach ( $breakdown[ $date ][ $fraud_code ] as $id => $nf ) {
												$link = $query_string . "&" . $date_qs . "&fraud_code=$fraud_code&detail=$id";
												$cnt  = $breakdown[ $date ][ $fraud_code ][ $id ][ 'count' ];
												$TABLE .= "
                          <tr>
                              <td>&nbsp;</td>
                              <td align='right'>
                                 <a href='$SCRIPT_NAME?$link'>$id</a>
                              </td>

                              <td align='right'>
                                 $cnt
                              </td>
                          </tr>
                       ";
										}
										$cnt = $dates[ $date ][ $fraud_code ][ count ];
										$TABLE .= "
                          <tr>
                              <td>&nbsp;</td>
                              <td align='right' style='border-top:1px solid silver;border-bottom:1px solid silver'>
                                 Subtotal
                              </td>

                              <td align='right' style='border-top:1px solid silver;border-bottom:1px solid silver'>
                                 $cnt
                              </td>
                          </tr>
                   ";
								} else {
										$cnt  = $dates[ $date ][ $fraud_code ][ count ];
										$link = $query_string . "&" . $date_qs . "&fraud_code=$fraud_code";
										$TABLE .= "
                          <tr>
                              <td align='right'>
                                 $show_date
                              </td>

                              <td align='right'>
                                 <b><a href='$SCRIPT_NAME?$link'>$fraud_code</a></b>
                              </td>

                              <td align='right'>
                                 $cnt
                              </td>
                          </tr>
                   ";
								}
						}
				}
				$report_form = jackhammer_report_form ();

				return "

           <table align='center' cellspacing='10'>
             <tr>
               <td valign='top' style='border-right:1px solid black'>
                  <b>$language[report_summary_header]</b><br />
                  <b>$language[date_header]</b> <i>$start_month/$start_day/$start_year - $end_month/$end_day/$end_year</i><br />
                  <b>$language[breakdown_prompt]</b> <i>$CGI[breakdown]</i><br />
                  <b>$language[acct_id_prompt]</b> <i>$account</i><br />
                  <b>$language[affiliate_prompt]</b> <i>$affiliate</i><br />
                  <b>$language[partner_prompt]</b> <i>$source</i><br />
               </td>
               <td valign='top'>
                  $report_form
               </td>
             </tr>
             <tr>
               <td style='border-bottom:1px solid black' colspan='2'></td>
             </tr>
           </table>

           <link rel='stylesheet' href='$config[script_dir]/tablesort.css'>
           <table align='center' class='sorted'>
               <thead>
               <tr>
                  <td>$language[date_header]</td>
                  <td>$language[fraud_code]</td>
                  <td>$language[fraud_count]</td>
               </tr>
               </thead>

               <tbody>

               $TABLE

               </tbody>

               <tr>
                  <td align='right' style='border-top:1px solid black'>
                     <b>Report Total</b>
                  </td>

                  <td colspan='2' align='right' style='border-top:1px solid black'>
                     $total_fraud
                  </td>

               </tr>


           </table>
       ";
		}

		function fraud_list ()
		{
				global $SCRIPT_NAME;
				global $language;
				global $oSQL;
				global $CGI;
				global $config;
				$end_day      = $CGI[ end_day ];
				$end_month    = $CGI[ end_month ];
				$end_year     = $CGI[ end_year ];
				$start_day    = $CGI[ start_day ];
				$start_month  = $CGI[ start_month ];
				$start_year   = $CGI[ start_year ];
				$downloadable = $CGI[ downloadable ];
				// Table Vars
				$click_fraud = CLICK_FRAUD_TABLE;
				## Force a minimal lookup, if sent in a detail id
				if ( $CGI[ breakdown ] == "source" && !$CGI[ source ] ) {
						$CGI[ source ] = $CGI[ detail ];
				}
				if ( $CGI[ breakdown ] == "affiliate" && !$CGI[ affiliate_id ] ) {
						$CGI[ affiliate_id ] = $CGI[ detail ];
				}
				$account    = $CGI[ acct_id ];
				$affiliate  = $CGI[ affiliate_id ];
				$source     = $CGI[ source ];
				$fraud_code = $CGI[ fraud_code ];
				## Pre-Populate the query string for detail links, with known info
				$query_string = "admin=hyperseek&action=report&report_type=fraud_list&breakdown=$CGI[breakdown]&acct_id=$account&affiliate_id=$affiliate&source=$source";
				$starttime    = $start_year . $start_month . $start_day;
				$endtime      = $end_year . $end_month . $end_day;
				## Build up the SQL ##
				$WHERE = '';
				if ( $account ) {
						$WHERE .= " AND account = '$account' ";
				}
				if ( $affiliate ) {
						$WHERE .= " AND affiliate = '$affiliate' ";
				}
				if ( $source ) {
						$WHERE .= " AND ppc_engine = '$source' ";
				}
				if ( $fraud_code ) {
						$WHERE .= " AND fraud_id = '$fraud_code' ";
				}
				$SQL         = "
          SELECT * FROM $click_fraud
          WHERE hit_date BETWEEN $starttime AND $endtime
          $WHERE
          ORDER BY hit_date
       ";
				$report_data = $oSQL->search ( array ( "SQL" => $SQL ) );
				## Need 2 table formats, for normal PPC ops and for Monthly listings
				$REPORT        = '';
				$TABLE         = '';
				$TABLE_MONTHLY = '';
				$CSV           = '';
				$dates         = array ();
				$affiliates    = array ();
				$sources       = array ();
				foreach ( $report_data as $hit ) {
						$date    = date ( "m/d/Y h:s" , $hit[ click_tstamp ] );
						$tbldate = date ( "Ymd" , $hit[ click_tstamp ] );
						$tbldate = substr ( $tbldate , 0 , 6 );
						$TABLE .= "
               <tr>
                  <td>
                     <a href='$SCRIPT_NAME?admin=hyperseek&action=report&report_type=view_fraud&hit_id=$hit[hit_id]&tbl=$tbldate'>View</a>
                  </td>
                  <td>
                     $date
                  </td>
                  <td>
                     $hit[fraud_id]
                  </td>
                  <td>
                     $hit[word]
                  </td>
                  <td>
                     $hit[price]
                  </td>
                  <td>
                     $hit[account]
                  </td>
                  <td>
                     $hit[affiliate]
                  </td>
                  <td>
                     $hit[ppc_engine]
                  </td>
                  <td>
                     $hit[ip_number]
                  </td>
               </tr>
           ";
				}
				$report_form = jackhammer_report_form ();

				return "
           <table align='center' cellspacing='10'>
             <tr>
               <td valign='top' style='border-right:1px solid black'>
                  <b>$language[report_summary_header]</b><br />
                  <b>$language[date_header]</b> <i>$start_month/$start_day/$start_year - $end_month/$end_day/$end_year</i><br />
                  <b>$language[breakdown_prompt]</b> <i>$CGI[breakdown]</i><br />
                  <b>$language[acct_id_prompt]</b> <i>$account</i><br />
                  <b>$language[affiliate_prompt]</b> <i>$affiliate</i><br />
                  <b>$language[partner_prompt]</b> <i>$source</i><br />
               </td>
               <td valign='top'>
                  $report_form
               </td>
             </tr>
             <tr>
               <td style='border-bottom:1px solid black' colspan='2'></td>
             </tr>
           </table>

           <link rel='stylesheet' href='$config[script_dir]/tablesort.css'>
           <table align='center' class='sorted'>
               <thead>
               <tr>
                  <td>&nbsp;</td>
                  <td>$language[fraud_timestamp]</td>
                  <td>$language[fraud_code]</td>
                  <td>$language[manage_bids_keyword]</td>
                  <td>$language[price_prompt]</td>
                  <td>$language[acct_id_prompt]</td>
                  <td>$language[affiliate_prompt]</td>
                  <td>$language[partner_prompt]</td>
                  <td>IP</td>
               </tr>
               </thead>

               <tbody>

               $TABLE

               </tbody>


           </table>
       ";
		}


		function view_click ()
		{
				global $SCRIPT_NAME;
				global $language;
				global $oSQL;
				global $CGI;
				global $config;
				$hit_id    = $CGI[ hit_id ];
				$table     = $CGI[ tbl ];
				$tablename = CLICK_LOG_TABLE;
				if ( $config[ jackhammer_merge_tables ] == "YES" ) {
						$tablename = $tablename . "_" . $table;
				}
				$SQL    = "SELECT * FROM $tablename WHERE hit_id = '$hit_id'";
				$click  = $oSQL->search ( array ( "SQL" => $SQL , "Return" => "hash" ) );
				$date   = date ( "m/d/Y h:s a" , $click[ hit_time ] );
				$output = "

          <br /><br />
          <table border=0 cellspacing=1 cellpadding=5 bgcolor='black'>

              <tr bgcolor='#e0e0e0'>
                    <td>Click ID</td><td>$click[hit_id]</td>
              </tr>

              <tr bgcolor='#e0e0e0'>
                    <td>Link ID</td><td>$click[link_id]</td>
              </tr>
              <tr bgcolor='#e0e0e0'>
                    <td>Advertiser:</td><td>$click[account]</td>
              </tr>
              <tr bgcolor='#e0e0e0'>
                    <td>Date</td><td>$date</td>
              </tr>
              <tr bgcolor='#e0e0e0'>
                    <td>Keyword</td><td>$click[word]</td>
              </tr>
              <tr bgcolor='#e0e0e0'>
                    <td>Bid Price</td><td>$click[price]</td>
              </tr>
              <tr bgcolor='#e0e0e0'>
                    <td>Affiliate</td><td>$click[affiliate]</td>
              </tr>
			  <tr bgcolor='#e0e0e0'>
                    <td>SubID</td><td>$click[subid]</td>
              </tr>
              <tr bgcolor='#e0e0e0'>
                    <td>IP Number</td><td>$click[ip]</td>
              </tr>
			  <tr bgcolor='#e0e0e0'>
                    <td>User Agent</td><td>$click[user_agent]</td>
              </tr>
              <tr bgcolor='#e0e0e0'>
                    <td>Referring URL</td><td><a href='$click[referer]' target='new'>$click[referer]</A></td>
              </tr>
             <tr bgcolor='#e0e0e0'>
                    <td>Country</td><td>$click[country]</td>
              </tr>

              <form action='$SCRIPT_NAME' method='post'>


              <tr bgcolor='#e0e0e0'>
                    <td valign='top'>Change/Select Fraud Code</td>
                    <td>
                             <select name='fraud_code'>
                                <option value='101'>101 : Searcher's IP doesn't match Clicker's IP
                                <option value='102'>102 : Double-Click (same link clicked twice)
                                <option value='103'>103 : Delayed Click (Click occurrs more than 5 minutes after search)
                                <option value='104'>104 : IP Number banned
                                <option value='105'>105 : Invalid Referer
                                <option value='106'>106 : Max # Clicks per IP Number reached
                                <option value='107'>107 : IP has seen this advertiser before (possibly a different keyword)
                                <option value='108'>108 : Person is logged in to their account
                                <option value='109'>109 : Country Code Violation (Language)
                                <option value='110'>110 : Invalid Account
                                <option value='888'>888 : Other
                                <option value='999'>999 : BOT
                             </select>
                    </td>
              </tr>

              <tr bgcolor='#E0E0E0'>
                    <td>Notes/Reason:</td>
                    <td><input name='notes' size='50'></td>
              </tr>

              <tr bgcolor='#e0e0e0'>
                    <td colspan='2' align='center'>
                             <input name='action' type='hidden' value='make_fraud'>
                             <input name='admin' type='hidden' value='hyperseek'>
                             <input name='hit_id' type='hidden' value='$click[hit_id]'>
                             <input name='tbl' type='hidden' value='$table'>
                             <input type='submit' value='Change Click to Fraud'>
                             <input type='reset'>
                    </td>
              </tr>

              </form>

          </TABLE>

        ";
				admin_output ( "Click Detail" , $output );
		}


		function view_fraud ()
		{
				global $SCRIPT_NAME;
				global $language;
				global $oSQL;
				global $CGI;
				global $config;
				$hit_id    = $CGI[ hit_id ];
				$table     = $CGI[ tbl ];
				$tablename = CLICK_FRAUD_TABLE;
				if ( $config[ jackhammer_merge_tables ] == "YES" ) {
						$tablename = $tablename . "_" . $table;
				}
				$SQL    = "SELECT * FROM $tablename WHERE hit_id = '$hit_id'";
				$click  = $oSQL->search ( array ( "SQL" => $SQL , "Return" => "hash" ) );
				$date   = date ( "m/d/Y h:s a" , $click[ click_tstamp ] );
				$output = "
          <table border=0 cellspacing=1 cellpadding=5 bgcolor='black'>

              <tr bgcolor='#e0e0e0'>
                    <td>Click ID</td><td>$click[hit_id]</td>
              </tr>

              <tr bgcolor='#e0e0e0'>
                    <td>Link ID</td><td>$click[link_id]</td>
              </tr>
              <tr bgcolor='#e0e0e0'>
                    <td>Advertiser:</td><td>$click[account]</td>
              </tr>
              <tr bgcolor='#e0e0e0'>
                    <td>Date</td><td>$date</td>
              </tr>
              <tr bgcolor='#e0e0e0'>
                    <td>Keyword</td><td>$click[word]</td>
              </tr>
              <tr bgcolor='#e0e0e0'>
                    <td>Bid Price</td><td>$click[price]</td>
              </tr>
              <tr bgcolor='#e0e0e0'>
                    <td>Affiliate</td><td>$click[affiliate]</td>
              </tr>
              <tr bgcolor='#e0e0e0'>
                    <td>IP Number</td><td>$click[ip_number]</td>
              </tr>
			  <tr bgcolor='#e0e0e0'>
                    <td>User Agent</td><td>$click[user_agent]</td>
              </tr>
              <tr bgcolor='#e0e0e0'>
                    <td>Referring URL</td><td><a href='$click[referer]' target='new'>$click[referer]</a></td>
              </tr>

              <tr bgcolor='#e0e0e0'>
                    <td>Fraud Code</td><td>$click[fraud_id]</td>
              </tr>

              <tr bgcolor='#e0e0e0'>
                    <td>Fraud Info</td><td>$click[fraud_reason]</td>
              </tr>

              <form actioN='$SCRIPT_NAME' method='post'>

              <tr bgcolor='#e0e0e0'>
                    <td colspan='2' align='center'>
                             <input name='action' type='hidden' value='make_click'>
                             <input name='admin' type='hidden' value='hyperseek'>
                             <input name='hit_id' type='hidden' value='$click[hit_id]'>
                             <input name='tbl' type='hidden' value='$table'>
                             <input type='submit' VALUE='Change To Valid Click'>
                             <input type='reset'>
                    </td>
              </tr>

              </form>

          </table>

        ";
				admin_output ( "Click Detail" , $output );
		}


		function search_stats ()
		{
				global $SCRIPT_NAME;
				global $language;
				global $oSQL;
				global $CGI;
				global $config;
				$now                 = time ();
				$then                = date ( "Ymd" , ( $now - ( 86400 * 30 ) ) );
				$month_num_searches  = 0;
				$month_num_cats      = 0;
				$total_num_searches  = 0;
				$total_num_cats      = 0;
				$summary_table       = LOG_SUMMARY_TABLE;
				$SQL                 = "
            SELECT sum(num_searches) AS num_searches,
                   sum(click_count) AS num_clicks,
                   sum(click_amount_gross) AS gross_revenue,
                   sum(click_amount_net) AS net_revenue
            FROM $summary_table
        ";
				$search_stats        = $oSQL->search ( array ( "SQL" => $SQL , "Return" => 'hash' ) );
				$average_click_gross = 0;
				if ( $search_stats[ gross_revenue ] && $search_stats[ num_clicks ] ) {
						$average_click_gross = $search_stats[ gross_revenue ] / $search_stats[ num_clicks ];
				}
				$average_click_net = 0;
				if ( $search_stats[ net_revenue ] && $search_stats[ num_clicks ] ) {
						$average_click_net = $search_stats[ net_revenue ] / $search_stats[ num_clicks ];
				}
				$ctr = 0;
				if ( $search_stats[ num_clicks ] ) {
						$ctr = $search_stats[ num_clicks ] / $search_stats[ num_searches ];
				}
				## Cleanup the numbers ....
				$average_click_gross = sprintf ( "%01.4f" , $average_click_gross );
				$average_click_net   = sprintf ( "%01.4f" , $average_click_net );
				$ctr                 = sprintf ( "%2.3f" , $ctr );
				$report              = "

          <link rel='stylesheet' href='$config[script_dir]/tablesort.css'>
          <table cellpadding='3' class='sorted'>
            <thead>
            <tr>
              <td>&nbsp;</td>
              <td>&nbsp;</td>
            </tr>
            </thead>
            <tbody>
            <tr>
              <td>$language[total_searches]</td>
              <td>$search_stats[num_searches]</td>
            </tr>
            <tr>
              <td>$language[total_clicks]</td>
              <td>$search_stats[num_clicks]</td>
            </tr>
            <tr>
              <td>$language[ctr]</td>
              <td>$ctr%</td>
            </tr>
            <tr>
              <td>$language[gross_revenue]</td>
              <td>
                  $language[dollar_sign] $search_stats[gross_revenue]
                  ( $language[dollar_sign] $average_click_gross avg )
              </td>
            </tr>
            <tr>
              <td>$language[net_revenue]</td>
              <td>
                  $language[dollar_sign] $search_stats[net_revenue]
                  ( $language[dollar_sign] $average_click_net avg )
              </td>
            </tr>
            </tbody>
          </table>
        ";
				admin_output ( $language[ report_search_stats ] , $report );
		}


		function keyword_performance ()
		{
				global $SCRIPT_NAME;
				global $language;
				global $oSQL;
				global $CGI;
				global $config;
				$num_per_page = 100;
				$start        = $CGI[ start ];
				if ( !$start ) {
						$start = 0;
				}
				$summary_table = LOG_SUMMARY_TABLE;
				$REPORT        = '';
				if ( $CGI[ 'categoriesonly' ] ) {
						$catwhere = "WHERE search_type = 'cat'";
						$SQL      = "SELECT full_name,category_id FROM " . CATEGORY_TABLE;
						$catlist  = $oSQL->search ( array ( "SQL" => $SQL , "Return" => "fullhash" ) );
				} else {
						$catwhere = "WHERE search_type = 'TEXTSEARCH'";
				}
				$ob = $CGI[ 'ob' ];
				if ( $ob ) {
						$order_by = "ORDER BY $ob DESC";
				}
				$SQL         = "
            SELECT count(*)
            FROM $summary_table
            $catwhere
        ";
				$num_records = $oSQL->search ( array ( "SQL" => $SQL , "Return" => "single" ) );
				$SQL         = "
            SELECT *,
                   IFNULL((click_amount_gross/click_count),0.00) AS gross_ppc,
                   IFNULL((click_amount_net/click_count),0.00) AS net_ppc
            FROM $summary_table
            $catwhere
            $order_by
            LIMIT $start,$num_per_page
        ";
				$stats       = $oSQL->search ( array ( "SQL" => $SQL ) );
				$ey          = date ( "Y" );
				$em          = date ( "m" );
				$ed          = date ( "d" );
				$sy          = '2000';
				$sm          = '01';
				$sd          = '01';
				$datestring  = "start_month=$sm&start_day=$sd&start_year=$sy&end_month=$em&end_day=$ed&end_year=$ey";
				foreach ( $stats as $keyword ) {
						$num_searches = number_format ( $keyword[ num_searches ] , 0 , '.' , ',' );
						$num_clicks   = number_format ( $keyword[ click_count ] , 0 , '.' , ',' );
						$gross_ppc    = sprintf ( '%01.4f' , $keyword[ gross_ppc ] );
						$net_ppc      = sprintf ( '%01.4f' , $keyword[ net_ppc ] );
						$criteria     = $keyword[ criteria ];
						$ekey         = urlencode ( $criteria );
						if ( $CGI[ 'categoriesonly' ] ) {
								$catid = $catlist[ $criteria ];
								$ekey  = urlencode ( "Category-$catid-$criteria" );
						}
						$link = $SCRIPT_NAME . "?admin=hyperseek&action=report&report_type=keyword_detail&keyword=$ekey&$datestring";
						$REPORT .= "
                <tr>
                  <td>
                     <a href='$link'>$criteria</a>
                  </td>
                  <td align='right'>
                     $num_searches
                  </td>
                  <td align='right'>
                     $num_clicks
                  </td>
                  <td align='right'>
                     $language[dollar_sign] $keyword[click_amount_gross]
                  </td>
                  <td align='right'>
                     $language[dollar_sign] $gross_ppc
                  </td>
                  <td align='right'>
                     $language[dollar_sign] $keyword[click_amount_net]
                  </td>
                  <td align='right'>
                     $language[dollar_sign] $net_ppc
                  </td>
                </tr>
            ";
				}
				// Generate Nav Bar //
				// Generate Nav Bar //
				if ( $CGI[ 'categoriesonly' ] ) {
						$type = "Categories";
						list( $num_pages , $nblinks ) = admin_report_nav_bar ( array ( "total" => $num_records , "num_per_page" => $num_per_page , "base_url" => "$SCRIPT_NAME?admin=hyperseek&action=keyword_performance&categoriesonly=1" , ) );
				} else {
						$type = 'Keywords';
						list( $num_pages , $nblinks ) = admin_report_nav_bar ( array ( "total" => $num_records , "num_per_page" => $num_per_page , "base_url" => "$SCRIPT_NAME?admin=hyperseek&action=keyword_performance&ob=$ob" , ) );
				}
				$nav_bar = "$num_records $type ($num_pages Pages): $nblinks";
				$report  = "
          <link rel='stylesheet' href='$config[script_dir]/tablesort.css'>

          <br /><br />

          <form action='$SCRIPT_NAME'>
             Re-Sort <select name='ob'>
               <option value='$CGI[ob]'>$CGI[ob]</option>
               <option value='num_searches'>$language[total_searches]</option>
               <option value='click_count'>$language[total_clicks]</option>
               <option value='click_amount_gross'>$language[gross_revenue]</option>
               <option value='gross_ppc'>$language[gross_ppc]</option>
               <option value='click_amount_net'>$language[net_revenue]</option>
               <option value='net_ppc'>$language[net_ppc]</option>
             </select>
             <input type='hidden' name='action' value='keyword_performance' />
             <input type='hidden' name='categoriesonly' value='$CGI[categoriesonly]' />
             <input type='hidden' name='admin' value='hyperseek' />
             <input type='submit' value='$language[report_button]' />
          </form>

          $nav_bar

          <table cellpadding='3' class='sorted'>
            <thead>
            <tr>
              <td type='CaseInsensitiveString'>$language[manage_bids_keyword]</td>
              <td type='Number'>$language[total_searches]</td>
              <td type='Number'>$language[total_clicks]</td>
              <td type='Number'>$language[gross_revenue]</td>
              <td type='Number'>$language[gross_ppc]</td>
              <td type='Number'>$language[net_revenue]</td>
              <td type='Number'>$language[net_ppc]</td>
            </tr>
            </thead>
            <tbody>
            $REPORT
            </tbody>
          </table>

          $nav_bar
        ";
				admin_output ( "$language[report_keywords]: $language[this_month]" , $report );
		}

		function admin_report_nav_bar ( $args = array () )
		{
				global $CGI;
				$total_items  = $args[ 'total' ];
				$base_url     = $args[ 'base_url' ];
				$num_per_page = $args[ 'num_per_page' ];
				$start        = $CGI[ 'start' ];
				// Previous Page Link //
				if ( $start > 1 ) {
						$pstart        = $start - $num_per_page;
						$previous_link = " <a href='$base_url&start=$pstart'>[Previous]</a> ";
				}
				// Next Page Link //
				if ( $start + $num_per_page < $total_items ) {
						$nstart    = $start + $num_per_page;
						$next_link = " <a href='$base_url&start=$nstart'>[Next]</a> ";
				}
				// Mid Span //
				$midspan   = '';
				$num_pages = floor ( $total_items / $num_per_page );
				if ( $total_items % $num_per_page ) {
						$num_pages++;
				}
				$this_page = floor ( $start / $num_per_page ) + 1;
				# Are we at the midpoint?
				if ( $this_page >= 5 ) {
						if ( $num_pages > ( $this_page + 5 ) ) {
								$start    = $this_page - 4;
								$end      = $this_page + 4;
								$nextpage = $end + 5;
								$prevpage = $start - 5;
						} else {
								$end = $num_pages;
								if ( $num_pages >= 10 ) {
										$start = $num_pages - 10;
								} else {
										$start = 1;
								}
						}
				} else {
						$start    = 1;
						$end      = 10;
						$nextpage = 11;
						$prevpage = "";
				}
				if ( $end > $num_pages ) {
						$end = $num_pages;
				}
				for ( $i = $start; $i <= $end; $i++ ) {
						if ( $i == $this_page ) {
								$link = " <b>$i</b> ";
						} else {
								$s    = ( $i - 1 ) * $num_per_page;
								$link = " <a href='$base_url&start=$s'>$i</a> ";
						}
						if ( $midspan ) {
								$midspan .= "|";
						}
						$midspan .= $link;
				}
				/*


				if ( $total_items > $num_per_page ) {
								$num_pages = floor($total_items / $num_per_page);
								if ( $total_items % $num_per_page ) { $num_pages++; }
								for ( $pagenum = 1; $pagenum <= $num_pages; $pagenum++ ) {
												$s = ($pagenum * $num_per_page)-$num_per_page;
												if ( $pagenum >1 ) { $nav_bar .= "|"; }
												$nav_bar .= " <a href='$base_url&start=$s'>$pagenum</a> ";
								}
				}

				*/
				$nav_bar = $previous_link . $midspan . $next_link;

				return array ( $num_pages , $nav_bar );
		}


		function trend_report ()
		{
				global $SCRIPT_NAME;
				global $language;
				global $oSQL;
				global $CGI;
				global $config;
				if ( preg_match ( "/interval/i" , $CGI[ breakdown ] ) ) {
						interval_report ();
				}
				// Table Vars
				$click_log = CLICK_LOG_TABLE;
				$per_page  = 100;
				$start     = $CGI[ start ];
				if ( !$start ) {
						$start = 0;
				}
				$end         = $start + $per_page - 1;
				$end_day     = $CGI[ end_day ];
				$end_month   = $CGI[ end_month ];
				$end_year    = $CGI[ end_year ];
				$start_day   = $CGI[ start_day ];
				$start_month = $CGI[ start_month ];
				$start_year  = $CGI[ start_year ];
				$date_qs     = "&start_month=$start_month&start_day=$start_day&start_year=$start_year&end_month=$end_month&end_day=$end_day&end_year=$end_year";
				$starttime   = $start_year . $start_month . $start_day;
				$endtime     = $end_year . $end_month . $end_day;
				$account     = $CGI[ acct_id ];
				$affiliate   = $CGI[ affiliate_id ];
				$source      = $CGI[ source ];
				## Build up the SQL ##
				$WHERE = '';
				if ( $account ) {
						$WHERE .= " AND account = '$account' ";
				}
				if ( $affiliate ) {
						$WHERE .= " AND affiliate = '$affiliate' ";
				}
				if ( $source ) {
						$WHERE .= " AND ppc_engine = '$source' ";
				}
				$REPORT   = '';
				$group_by = "GROUP BY $CGI[breakdown] ORDER BY $CGI[breakdown]";
				$min      = $CGI[ minimum ];
				if ( !$min ) {
						$min = 2;
				}
				$SQL         = "
            SELECT $CGI[breakdown] AS criteria, count(*) AS cnt FROM $click_log
            WHERE hit_date BETWEEN $starttime AND $endtime
            $WHERE
            GROUP BY $CGI[breakdown]
            HAVING count(*) >= $min
            ORDER BY $CGI[breakdown]
        ";
				$stats       = $oSQL->search ( array ( "SQL" => $SQL ) );
				$num_records = count ( $stats );
				if ( $num_records < $end ) {
						$end = $num_records;
				}
				$report_stats = array_slice ( $stats , $start , $end );
				foreach ( $report_stats as $item ) {
						if ( !$item[ criteria ] ) {
								$item[ criteria ] = "n/a";
						}
						$REPORT .= "
                <tr>
                  <td>
                     $item[criteria]
                  </td>
                  <td align='right'>
                     $item[cnt]
                  </td>
                </tr>
            ";
				}
				$nav_bar = "$num_records Found ";
				if ( $num_records > $per_page ) {
						$num_pages = ceil ( $num_records / $per_page );
						if ( $num_pages ) {
								$nav_bar .= ": $num_pages Pages ... ";
						}
						for ( $pagenum = 1; $pagenum <= $num_pages; $pagenum++ ) {
								$s = $pagenum * $per_page;
								$s -= $per_page;
								if ( $pagenum > 1 ) {
										$navbar .= "|";
								}
								$nav_bar .= " <a href='$SCRIPT_NAME?admin=hyperseek&action=report&report_type=trend_report&breakdown=$CGI[breakdown]&start=$s$date_qs'>$pagenum</a> ";
						}
						$nav_bar .= "<br /><br />";
				}
				$report_form = jackhammer_report_form ();
				$report      = "
          <link rel='stylesheet' href='$config[script_dir]/tablesort.css'>
          <table cellspacing='10'>
             <tr>
                <td valign='top' style='border-right:1px solid silver'>
                  <b>$language[report_summary_header]</b><br />
                  <b>$language[date_header]</b> <i>$start_month/$start_day/$start_year - $end_month/$end_day/$end_year</i><br />
                  <b>$language[breakdown_prompt]</b> <i>$CGI[breakdown]</i><br />
                  <b>$language[acct_id_prompt]</b> <i>$account</i><br />
                  <b>$language[affiliate_prompt]</b> <i>$affiliate</i><br />
                  <b>$language[partner_prompt]</b> <i>$source</i><br />
                </td>
                <td valign='top'>
                   $report_form
                </td>
             </tr>
          </table>

          <br /><br />
          $nav_bar

          <table cellpadding='3' class='sorted'>
            <thead>
            <tr>
              <td type='CaseInsensitiveString'>$language[trend_criteria]</td>
              <td type='Number'>$language[trend_count]</td>
            </tr>
            </thead>
            <tbody>
            $REPORT
            </tbody>
          </table>

        ";
				admin_output ( "$language[trend_report]: $CGI[breakdown]" , $report );
		}


		function interval_report ()
		{
				global $SCRIPT_NAME;
				global $language;
				global $oSQL;
				global $CGI;
				global $config;
				// Table Vars
				$click_log = CLICK_LOG_TABLE;
				$per_page  = 100;
				$start     = $CGI[ start ];
				if ( !$start ) {
						$start = 0;
				}
				$end         = $per_page;
				$end_day     = $CGI[ end_day ];
				$end_month   = $CGI[ end_month ];
				$end_year    = $CGI[ end_year ];
				$start_day   = $CGI[ start_day ];
				$start_month = $CGI[ start_month ];
				$start_year  = $CGI[ start_year ];
				$date_qs     = "&start_month=$start_month&start_day=$start_day&start_year=$start_year&end_month=$end_month&end_day=$end_day&end_year=$end_year";
				$starttime   = $start_year . $start_month . $start_day;
				$endtime     = $end_year . $end_month . $end_day;
				$account     = $CGI[ acct_id ];
				$affiliate   = $CGI[ affiliate_id ];
				$source      = $CGI[ source ];
				## Build up the SQL ##
				$WHERE = '';
				if ( $account ) {
						$WHERE .= " AND account = '$account' ";
				}
				if ( $affiliate ) {
						$WHERE .= " AND affiliate = '$affiliate' ";
				}
				if ( $source ) {
						$WHERE .= " AND ppc_engine = '$source' ";
				}
				$REPORT   = '';
				$group_by = "GROUP BY $CGI[breakdown] ORDER BY $CGI[breakdown]";
				$min      = $CGI[ minimum ];
				if ( !$min ) {
						$min = 2;
				}
				$cSQL        = "
            SELECT count(*) FROM $click_log
            WHERE hit_date BETWEEN $starttime AND $endtime
            $WHERE
        ";
				$num_records = $oSQL->search ( array ( "SQL" => $cSQL , "Return" => "single" ) );
				$SQL         = "
            SELECT * FROM $click_log
            WHERE hit_date BETWEEN $starttime AND $endtime
            $WHERE
            ORDER BY hit_time ASC
            LIMIT $start, $end
        ";
				$stats       = $oSQL->search ( array ( "SQL" => $SQL ) );
				$last_time   = 0;
				foreach ( $stats as $click ) {
						$time     = $click[ hit_time ];
						$interval = "0:00";
						if ( $last_time ) {
								$diff = $time - $last_time;
								if ( $diff > 60 ) {
										$interval_minutes = ceil ( $diff / 60 );
										$interval_seconds = $diff % 60;
										if ( $interval_seconds < 10 ) {
												$interval_seconds = "0" . $interval_seconds;
										}
										$interval = $interval_minutes . ":" . $interval_seconds;
								} else {
										if ( $diff < 10 ) {
												$diff = "0" . $diff;
										}
										$interval = ":" . $diff;
								}
						}
						$stamp = date ( "m/d/Y h:i:s a" , $time );
						## save for next go'round
						$last_time = $time;
						$date      = $click[ hit_date ];
						$date      = substr ( $date , 0 , 6 );
						$REPORT .= "
               <tr>
                  <td>
                     <a href='$SCRIPT_NAME?admin=hyperseek&action=report&report_type=view_click&hit_id=$click[hit_id]&tbl=$date'>Details / Tag as Fraud</a>
                  </td>
                  </td>
                  <td>$stamp</td>
                  <td align='right'><b>$interval</b></td>
                  <td>$click[ip]</td>
                  <td>$click[ppc_engine]</td>
                  <td>$click[affiliate]</td>
                  <td>$click[word]</td>
                  <td>$click[price]</td>
               </tr>
            ";
				}
				$nav_bar = "$num_records Found ";
				if ( $num_records > $per_page ) {
						$num_pages = floor ( $num_records / $per_page );
						if ( $num_pages ) {
								$nav_bar .= ": $num_pages Pages ... ";
						}
						for ( $pagenum = 1; $pagenum <= $num_pages; $pagenum++ ) {
								$s = $pagenum * $per_page;
								$s -= $per_page;
								if ( $pagenum > 1 ) {
										$nav_bar .= "|";
								}
								$nav_bar .= " <a href='$SCRIPT_NAME?admin=hyperseek&action=report&report_type=trend_report&breakdown=$CGI[breakdown]&start=$s$date_qs'>$pagenum</a> ";
						}
						$nav_bar .= "<br /><br />";
				}
				$report_form = jackhammer_report_form ();
				$OUTPUT      = "
          <link rel='stylesheet' href='$config[script_dir]/tablesort.css'>

          <table cellspacing='10'>
             <tr>
                <td valign='top' style='border-right:1px solid silver'>
                  <b>$language[report_summary_header]</b><br />
                  <b>$language[date_header]</b> <i>$start_month/$start_day/$start_year - $end_month/$end_day/$end_year</i><br />
                  <b>$language[breakdown_prompt]</b> <i>$CGI[breakdown]</i><br />
                  <b>$language[acct_id_prompt]</b> <i>$account</i><br />
                  <b>$language[affiliate_prompt]</b> <i>$affiliate</i><br />
                  <b>$language[partner_prompt]</b> <i>$source</i><br />
                </td>
                <td valign='top'>
                   $report_form
                </td>
             </tr>
          </table>

          <br /><br />
          $nav_bar

          <table cellpadding='3' class='sorted'>
            <thead>
               <tr>
                  <td>Action</td>
                  <td>Time Stamp</td>
                  <td>Interval</td>
                  <td>IP Address</td>
                  <td>Source</td>
                  <td>Affiliate</td>
                  <td>Keyword</td>
                  <td>Price</td>
               </tr>
            </thead>
            <tbody>
            $REPORT
            </tbody>
          </table>

        ";
				admin_output ( "$language[trend_report]: $CGI[breakdown]" , $OUTPUT );
		}


?>
