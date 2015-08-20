<?php
  // module_load_include('inc', 'lumf', 'user_helper');


  function create_or_update_role($name,$permissions = array(),$options = array()){
    $role = user_role_load_by_name($name); 
    if(empty($role)){
      $role = new stdClass();
      $role->name = $name;
      foreach($options as $key => $val){
        $role->{$key} = $val;
      }
      user_role_save($role);
      $role = user_role_load_by_name($name); 
    }else{
      $edited = false;
      foreach($options as $key => $val){
        if(!isset($role->{$key}) || $role->{$key} !== $val){
          $role->{$key} = $val;
          $edited = true;
        }
      }
      if($edited){
        user_role_save($role);
      }
    }
    $modules = user_permission_get_modules();
    $final_perm = array();
    foreach((array)$permissions as $key => $val){
      if(is_numeric($key)){
        $key = $val;
        $val = true;
      }
      if(!empty($modules[$key])){
        $final_perm[$key] = $val;
      }
    }
    user_role_change_permissions($role->rid, $final_perm);
    return $role;
  }
  
  function create_or_update_user($name,$options = array()){
    $user = user_load_by_name($name); 
    if(!empty($options['roles'])){
      $roles = array();
      foreach((array)$options['roles'] as $key => $val){
        if(!empty($val)){
          $role = user_role_load_by_name($val); 
          $roles[$role->rid] = $val;
        }
      }
      $options['roles'] = $roles;
    }
    if(empty($user)){
      $default = array(
        'status' => 1,
      );
      $opt = array_merge($default,$options);
      $user = new stdClass();
      $user->name = $name;
      foreach($opt as $key => $val){
        $user->{$key} = $val;
      }
      user_save($user);
      $user = user_load_by_name($name); 
    }else{
      $edited = false;
      module_load_include('inc', 'lumf', 'array_helper');
      foreach($options as $key => $val){
        if(!isset($user->{$key}) || !options_are_eq($val,$user->{$key})){
          $user->{$key} = $val;
          $edited = true;
        }
      }
      if($edited){
        user_save($user);
      }
    }
    return $user;
  }