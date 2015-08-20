<?php
  // module_load_include('inc', 'lumf', 'custom_block_helper');
  
  function create_or_update_custom_block($name,$info){
    $block = named_custom_block_load($name);
    $fields = array('body', 'info', 'format');
    if(empty($block)){
      $default = array(
        'machine_name'=>$name,
        'format' => 'filtered_html',
      );
      $data = array_merge($default,array_intersect_key($info,array_flip($fields)));
      $bid = db_insert('block_custom')->fields($data)->execute();
    }else{
      $bid = $block['bid'];
      $data = array_merge($block,array_intersect_key($info,array_flip($fields)));
      $data['body'] = array(
        'value' => $data['body'],
        'format' => $data['format'],
      );
      block_custom_block_save($data,$bid);
    }
    if(!empty($info['regions']) && !empty($bid)){
      foreach ($info['regions'] as $theme => $region) {
        db_merge('block')->key(array(
          'theme' => $theme,
          'delta' => $bid,
          'module' => 'block'
        ))->fields(array(
          'region' => ($region == BLOCK_REGION_NONE ? '' : $region),
          'pages' => '',
          'status' => (int) ($region != BLOCK_REGION_NONE),
        ))->execute();
      }
    }
  }