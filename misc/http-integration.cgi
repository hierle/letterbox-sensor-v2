#!/usr/bin/perl -w

# ttn / chirpstack http integration
use English;
use strict;
use Socket;


my $threshold=150;

my $nowint = time;
my $nowstr = int2time($nowint);
my $today = `date +%Y%m%d`;
my $reqm = $ENV{'REQUEST_METHOD'};
#if ((!defined $reqm)||($reqm ne "GET")) { response(400,"illegal request method"); exit; }
my $logfile = "/home/ttn/public_html/ttn/letterbox-sensor.$today";
my $statusfile = "/home/ttn/public_html/ttn/letterbox-sensor.status";
my $lastfile = "/home/ttn/public_html/ttn/letterbox-sensor.last";

if ($reqm eq "POST") {
 # write log file
 open FH, ">> $logfile";
 while (<STDIN>) { print FH $_ . "\n"; }
 close FH;
 
 response(200,"OK");

 my $laststatus = `cat $statusfile`;
 my $status = 0;
 my ($sensor1,$sensor2,$voltage) = &getsensors();
 if(($sensor1 > $threshold)||($sensor2 > $threshold)) { $status=1; }
 if($laststatus != $status) {
  open FH, "> $statusfile";
  print FH $status;
  close FH;
 }
 my $last = `tail -1 $logfile`;
 if($last =~ /OTAA/) {
   open FH, "> $lastfile";
   print FH "$sensor1\n$sensor2\n$voltage\n";
   close FH;
 }
}
else {
 my $sage = &getage($statusfile);
 my $lage = &getage($lastfile);
 my ($sensor1,$sensor2,$voltage) = `cat $lastfile`;
 if(($sensor1 > $threshold)||($sensor2 > $threshold)) { 
   &letter("green","Sensor1&nbsp;: $sensor1<br>Sensor2&nbsp;: $sensor2<br>Voltage&nbsp;: $voltage V<br>Change: $sage ago<br>LastRX&nbsp;: $lage ago"); }
 else { 
   &letter("grey","Sensor1&nbsp;: $sensor1<br>Sensor2&nbsp;: $sensor2<br>Voltage&nbsp;: $voltage V<br>Change: $sage ago<br>LastRX&nbsp;: $lage ago"); }
}

exit;



################
sub getsensors {
 my $cmd = "tail -1 $logfile";
 # get sensor values just for one specific sensor name
 #my $cmd = "awk '/sens-1/ {a=\$0} END{print a}' $logfile";
 my $lastline = `$cmd`;
 my $sensor1=0;
 my $sensor2=0;
 my $voltage=0;
 if($lastline =~ /\\"sensor1\\":(.*?)[,\}].*/) { $sensor1=$1; }
 if($lastline =~ /\\"sensor2\\":(.*?)[,\}].*/) { $sensor2=$1; }
 if($lastline =~ /\\"voltage\\":(.*?)[,\}].*/) { $voltage=$1; }
 return($sensor1,$sensor2,$voltage);
}

############
sub getage {
 my $file = shift;
 chomp $file;

 my $age = -M $file;
 my $sec = $age * 24 * 60 * 60;
 my $days = int ($sec/24/60/60);
 my $hours = int (($sec - $days*24*60*60)/60/60);
 if($hours < 10) { $hours = "0$hours"; }
 my $min=int(($sec-$days*24*60*60-$hours*60*60)/60);
 if($min < 10) { $min = "0$min"; }
 my $seconds=int($sec-$days*24*60*60-$hours*60*60-$min*60);
 if($seconds < 10) { $seconds = "0$seconds"; }
 my $astring = "${hours}:${min}:${seconds}";
 if($days > 0) { $astring = "${days}d " . $astring; }
 return $astring;
}


##############
sub response {
  my $status = shift;
  my $message = shift;

  my $html = "Status: $status
Date: $nowstr
Last-modified: $nowstr
Pragma: no-cache
Cache-control: no-cache
Content-Type: text/html; charset=utf-8

<html><head></head><body>$message</body></html>
\n";
  print $html;
}

############
sub letter {
  my $bg = shift;
  my $message = shift;

  my $html = "Status: 200 OK
Date: $nowstr
Last-modified: $nowstr
Pragma: no-cache
Cache-control: no-cache
Content-Type: text/html; charset=utf-8

<html><head></head><body bgcolor=$bg><font size=+3 face=verdana><b>$message</b></font></body></html>
\n";
  print $html;
}

##############
sub int2time {
    my $nowint=$ARG[0];
    if (!defined $nowint) { $nowint = time; }
    my $nowstr;
    $nowstr = gmtime($nowint);
    if ( $nowstr =~ /^(...)\ (...)\ (..)\ (..:..:..)\ (....)$/ ) {
         $nowstr = "$1\, $3 $2 $5 $4 GMT";
    }
    return $nowstr;
}

