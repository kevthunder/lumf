<?php 
  use ICanBoogie\Inflector;
      
  class LumfBaseRenameOptions extends RenameOptions{
    
    var $checks = array(
      '/\.(php|inc|module|info|install|md)$/' => 'replace_model_name',
      '/\.info$/' => 'remove_hide',
    );
    var $checkRendered = true;
    
    function helper_name(){
      $inflector = Inflector::get();
      return $this->process->prompt(array(
        'prompt'=>'Helper name',
        'default'=>$this->process->lastChoice('helper_name',$inflector->camelize(preg_replace('/(^lumf_)|(_lumf$)/','',$this->process->dest_module)))
      ),'helper_name');
    }
    
    function remove_hide($file){
      $str = file_get_contents($file);
      $str = str_replace('hidden = TRUE', '',$str);
      file_put_contents($file, $str);
    }
    
    function replace_model_name($file){
      $str = file_get_contents($file);
      $str = str_replace('HelperName', $this->helper_name(),$str);
      
      $str = $this->process->replace_promp('primary_key_field', array('prompt'=>'Primary key for you model','default'=>$this->process->lastChoice('primary_key_field',$model_name[0].'id')),$str,true);
      
      $inflector = Inflector::get();
      $def = 'Less UI More Fun with '.$inflector->humanize($this->helper_name());
      $str=$this->process->replace_promp('[todo] lumf helper name', array('prompt'=>'Module Human Name','default'=>$this->process->lastChoice('title',$def)),$str,'helper_title');
      $str=$this->process->replace_promp('[todo] lumf helper description', array('prompt'=>'Module description','default'=>$this->process->lastChoice('description',$def)),$str,'helper_description');
    
      file_put_contents($file, $str);
    }
  }
 ?>