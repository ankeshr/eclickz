<?php

		/*
		  		________________________________________________________________________________

						  PROGRAM : ASP : Admin : Affiliate Program Admin Center (PHP)
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
		$PROGRAMS[ 'jackhammer_affiliates' ] = "affiliate";

		function config_jackhammer_affiliates ()
		{
				global $oSQL;
				global $config;
				global $language;
				global $vars;
				global $menuitems;
				global $ADMIN_TYPE;
				$vars[ 'jackhammer_affiliates' ][ '01' ]                                       = array ( "Affiliate System Options." , "HEADER" , "" , "affiliate" );
				$vars[ 'jackhammer_affiliates' ][ '07:jackhammer_affiliate_payment_options' ]  = array ( "Allow affiliates to choose how they will get paid? (Cash/Check or Bidding acct deposit)" , "SELECT" , "YES|NO" , "affiliate" );
				$vars[ 'jackhammer_affiliates' ][ '08:jackhammer_affiliate_payment_default' ]  = array ( "If not, what is the default?" , "SELECT" , "cash|deposit" , "affiliate" );
				$vars[ 'jackhammer_affiliates' ][ '09:jackhammer_affiliate_search_payment' ]   = array ( "Default Amount (dd.cc) to pay affiliates per search" , "TEXT" , "" , "affiliate" );
				$vars[ 'jackhammer_affiliates' ][ '10:jackhammer_affiliate_click_payment' ]    = array ( "Default amount (dd.cc or nn\%) to pay affiliates per click" , "TEXT" , "" , "affiliate" );
				$vars[ 'jackhammer_affiliates' ][ '11:jackhammer_affiliate_revshare_payment' ] = array ( "Default amount (dd.cc or nn\%) to pay affiliates per referral deposit" , "TEXT" , "" , "affiliate" );
				$vars[ 'jackhammer_affiliates' ][ '12:jackhammer_affiliate_minimums' ]         = array ( "Allow affiliates to request a minimum payment amount?" , "SELECT" , "YES|NO" , "affiliate" );
				$vars[ 'jackhammer_affiliates' ][ '13:jackhammer_affiliate_minimum_options' ]  = array ( "List of minimum payments (in dd.cc format) you will allow them to select from (if yes to above option)... separate amounts with commas" , "TEXT" , "" , "affiliate" );
				$vars[ 'jackhammer_affiliates' ][ '14:jackhammer_max_daily_searches' ]         = array ( "Maximum number of searches to pay affiliates on per day" , "TEXT" , "" , "affiliate" );
				$vars[ 'jackhammer_affiliates' ][ '15:jackhammer_max_daily_clicks' ]           = array ( "Maximum number of clicks to pay affiliates on per day" , "TEXT" , "" , "affiliate" );
				$vars[ 'jackhammer_affiliates' ][ '16:jackhammer_auto_approve_affiliates' ]    = array ( "Auto-approve Affiliates?" , "SELECT" , "YES|NO" , "affiliate" );
				$vars[ 'jackhammer_affiliates' ][ '17:jackhammer_feed_price' ]                 = array ( "XML Feed should show NET or GROSS price" , "SELECT" , "NET|GROSS" , "affiliate" );
				$vars[ 'URLS' ][ '16:affjump_url' ]                                            = array ( "Full URL (with http://) to the search program for affiliate search boxes to use" , "TEXT" , "" , "affiliate" );
		}

?>
