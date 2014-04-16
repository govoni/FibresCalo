use Env;



open(USERCONFIG,$ARGV[0]);
while(<USERCONFIG>)
{
  chomp;
  s/#.*//;                # no comments
  s/^\s+//;               # no leading white
  s/\s+$//;               # no trailing white
  my($var,$value) = split(/\s*=\s*/, $_, 2);
  $User_Preferences{$var} = $value;
}

$OUTPUTDir = $User_Preferences{"OUTPUTDir"};



open(LIST,"./list_FNAL14.txt");
#open(LIST,"./list_Full_Lisbon.txt");
while(<LIST>)
{
  chomp;
  s/#.*//;                # no comments
  s/^\s+//;               # no leading white
  s/\s+$//;               # no trailing white
  
  ($label,$particle,$energy,$preL,$Nevts,$Njobs,$Nfirst) = split(" ");
  
  if( $label eq "" )
  {
    next;
  }
  
  if( $preL > 0. )
  {
    $label = $label."_preshower".$preL."mm";
  }
  print($label." ".$particle," ".$energy." ".$preL." ".$Nevts." ".$Njobs." ".$Nfirst."\n");
  
  
  $runDir = $OUTPUTDir."/run_".$label."/";
  $command = "hadd -f ".$runDir."/out.root ";
  
  for($iJob = $Nfirst; $iJob < $Nfirst+$Njobs; ++$iJob)
  {
    $jobDir = $runDir."/job_".$iJob."/";
    if( ! -e $jobDir )
    {
      print("job directory ".$jobDir." doesn't exist. Exiting...\n");
      exit;
    }
    else
    {
      $command = $command." ".$jobDir."/out_".$iJob.".root";
    }
  }
  
  #print($command."\n");
  system($command);
}
