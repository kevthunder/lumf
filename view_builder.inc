<?php 
  // module_load_include('inc', 'lumf', 'view_builder');
  // new ViewBuilder($name,$base_table)
class ViewBuilder {
  function __construct($name,$base_table = 'node') {
    $this->name = $name;
    $this->base_table = $base_table;
  }
  function view(){
    if(!empty($this->view)) return $this->view;
    $this->view = views_get_view('test');
    if(empty($this->view)) {
      $this->view = new view();
      $this->view->name = $this->name;
      $this->view->base_table = $this->base_table;
    }
    return $this->view;
  }
  function default_display(){
    if(!empty($this->view()->display['default'])) return $this->view()->display['default'];
    
    $handler = $this->view()->new_display('default', 'Master', 'default');
    if($this->base_table == 'node'){
      $handler->display->display_options['row_plugin'] = 'node';
      $handler->display->display_options['filters']['status'] = array(
        'id' => 'status',
        'table' => 'node',
        'field' => 'status',
        'value' => 1,
      );
    }
    return $handler->display;
  }
  function page_display($id = 'page'){
    if(!empty($this->view()->display[$id])) return $this->view()->display[$id];
    
    $handler = $this->view()->new_display('page', 'Page', $id);
    $handler->display->display_options['path'] = $this->name;
    return $handler->display;
  }
  function conditions($conds){
    foreach ($conds as $key => $opt) {
      $opt = $this->normaliseCond($opt,$key);
      $this->default_display()->display_options['filters'][$key] = $opt;
    }
  }
  function normaliseCond($cond,$key){
    if(!is_array($cond)){
      $cond = array(
        'id' => $key,
        'table' => $this->base_table,
        'field' => $key,
        'value' => $cond,
      );
    }
    if(empty($cond['table'])) $cond['table'] = $this->base_table;
    if($this->base_table == 'node' && $cond['field'] == 'type' && !is_array($cond['value'])) {
      $cond['value'] = array( $cond['value'] => $cond['value'] );
    }
    return $cond;
  }
  
  function save(){
    return $this->view()->save();
  }
}

?>