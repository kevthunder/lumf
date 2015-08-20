<?php
  // module_load_include('inc', 'lumf', 'block_helper');
  
  function set_block_config($options){
    if(!array_key_exists('status',$options) && array_key_exists('region',$options)){
      $options['status'] = 1;
    }
    $defOpt = array(
      'module' => 'block',
      'region' => BLOCK_REGION_NONE,
      'pages' => '',
    );
    $opt = array_merge($defOpt,$options);
    if(!empty($opt['theme']) && !empty($opt['delta']) && !empty($opt['module'])){
      $defOpt['status'] = (int) ($opt['region'] != BLOCK_REGION_NONE && !empty($opt['region']));
      $config = db_query( "SELECT * FROM {block} WHERE theme = :theme AND delta = :delta AND module = :module", array(
        ':theme' => $opt['theme'],
        ':delta' => $opt['delta'],
        ':module' => $opt['module']
      ))->fetchAssoc();
      if(!empty($config)){
        $opt = array_merge($defOpt,$config,$options);
      }else{
        $opt = array_merge($defOpt,$options);
      }
      
      $findKeys = array('theme', 'delta', 'module');
      db_merge('block')->key(
        array_intersect_key($opt,array_flip($findKeys))
      )->fields(
        array_diff_key($opt,array_flip($findKeys))
      )->execute();
    }
  }