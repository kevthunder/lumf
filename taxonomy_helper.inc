<?php
  // module_load_include('inc', 'lumf', 'taxonomy_helper');
  
  function create_or_update_taxonomy($info){
    $taxonomy = taxonomy_vocabulary_machine_name_load($info['machine_name']);
    if($taxonomy) $info = array_merge((array)$taxonomy,$info);
    
    module_load_include('inc', 'lumf', 'array_helper');
    
    if(!$taxonomy || !options_are_eq($info,(array)$taxonomy)){
      return taxonomy_vocabulary_save($info);
    }
  }
?>