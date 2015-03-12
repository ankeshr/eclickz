<?php

		/*
		________________________________________________________________________________

						PROGRAM : ASP : Admin : Payment Gateways Admin Center (PHP)
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
		$PROGRAMS[ 'payment_gateway' ] = 1;
		function config_payment_gateway ()
		{
				global $oSQL;
				global $config;
				global $language;
				global $vars;
				global $menuitems;
				$vars[ 'Payments' ][ '00' ]                      = array ( "Payment and Merchant Settings" , "HEADER" , "" );
				$vars[ 'Payments' ][ '01:payment_gateway' ]      = array ( "Select Gateway" , "SELECT" , "linkpoint|authnet|bofa|verisign" );
				$vars[ 'Payments' ][ '10' ]                      = array ( "Authorize.net Settings" , "HEADER" , "" );
				$vars[ 'Payments' ][ '11:authnet_login' ]        = array ( "Authorize.net Account ID" , "TEXT" , "" );
				$vars[ 'Payments' ][ '12:authnet_secret' ]       = array ( "Authorize.net Transaction Key" , "TEXT" , "" );
				$vars[ 'Payments' ][ '20' ]                      = array ( "Linkpoint Settings" , "HEADER" , "" );
				$vars[ 'Payments' ][ '21:linkpoint_storename' ]  = array ( "Linkpoint Store Number" , "TEXT" , "" );
				$vars[ 'Payments' ][ '22:linkpoint_pem' ]        = array ( "Linkpoint PEM File (Full Path)" , "TEXT" , "" );
				$vars[ 'Payments' ][ '23:linkpoint_server' ]     = array ( "Linkpoint Server<br />(Choose 'staging' for testing, 'secure' for live transactions)" , "SELECT" , "staging|secure" );
				$vars[ 'Payments' ][ '24:linkpoint_comm_type' ]  = array ( "Linkpoint Communication Type<br />(Choose 'curl' if your PHP has the CURL library installed)" , "SELECT" , "standard|curl" );
				$vars[ 'Payments' ][ '30' ]                      = array ( "Bank Of America Settings" , "HEADER" , "" );
				$vars[ 'Payments' ][ '31:bofa_merchant_id' ]     = array ( "Merchant ID" , "TEXT" , "" );
				$vars[ 'Payments' ][ '40' ]                      = array ( "Verisign Settings" , "HEADER" , "" );
				$vars[ 'Payments' ][ '41:verisign_login' ]       = array ( "Verisign Login ID" , "TEXT" , "" );
				$vars[ 'Payments' ][ '42:verisign_password' ]    = array ( "Verisign Password" , "TEXT" , "" );
				$vars[ 'Payments' ][ '90' ]                      = array ( "Payment Form HTML" , "HEADER" , "" );
				$vars[ 'Payments' ][ '91:jackhammer_order_url' ] = array ( "The URL/FORM to link to your order form ( use &lt;&lt;ACCOUNT&gt;&gt; and &lt;&lt;AMOUNT&gt;&gt; to insert the account id and deposit amount in the URL " , "TEXTAREA" , "/cgi-bin/search/accounts/orderform.cgi" , "design" );
		}

?>