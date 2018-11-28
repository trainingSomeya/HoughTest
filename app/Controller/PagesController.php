<?php
/**
 * Static content controller.
 *
 * This file will render views from views/pages/
 *
 * CakePHP(tm) : Rapid Development Framework (https://cakephp.org)
 * Copyright (c) Cake Software Foundation, Inc. (https://cakefoundation.org)
 *
 * Licensed under The MIT License
 * For full copyright and license information, please see the LICENSE.txt
 * Redistributions of files must retain the above copyright notice.
 *
 * @copyright     Copyright (c) Cake Software Foundation, Inc. (https://cakefoundation.org)
 * @link          https://cakephp.org CakePHP(tm) Project
 * @package       app.Controller
 * @since         CakePHP(tm) v 0.2.9
 * @license       https://opensource.org/licenses/mit-license.php MIT License
 */

App::uses('AppController', 'Controller');

/**
 * Static content controller
 *
 * Override this controller by placing a copy in controllers directory of an application
 *
 * @package       app.Controller
 * @link https://book.cakephp.org/2.0/en/controllers/pages-controller.html
 */
class PagesController extends AppController {

/**
 * This controller does not use a model
 *
 * @var array
 */
  public $uses = array();

/**
 * Displays a view
 *
 * @return CakeResponse|null
 * @throws ForbiddenException When a directory traversal attempt.
 * @throws NotFoundException When the view file could not be found
 *   or MissingViewException in debug mode.
 */
  public function display() {
    $path = func_get_args();

    $count = count($path);
    if (!$count) {
      return $this->redirect('/');
    }
    if (in_array('..', $path, true) || in_array('.', $path, true)) {
      throw new ForbiddenException();
    }
    $page = $subpage = $title_for_layout = null;

    if (!empty($path[0])) {
      $page = $path[0];
    }
    if (!empty($path[1])) {
      $subpage = $path[1];
    }
    if (!empty($path[$count - 1])) {
      $title_for_layout = Inflector::humanize($path[$count - 1]);
    }
    $this->set(compact('page', 'subpage', 'title_for_layout'));

    try {
      $this->render(implode('/', $path));
    } catch (MissingViewException $e) {
      if (Configure::read('debug')) {
        throw $e;
      }
      throw new NotFoundException();
    }
  }
  public function image() {
      $filepath = Router::url();// /read/image/imgName/0/imgName_tile_y_tile_x_mag.png
      $len = strlen($filepath);//
      $filepath = substr($filepath, 6, $len);// image/imgName/0/imgName_tile_y_tile_x_mag.png
      
      $fileInfo = explode('/', $filepath);//[image, imgName, 0, imgName_tile_y_tile_x_mag.png]
      $fileName = $fileInfo[count($fileInfo) - 1];// imgName_tile_y_tile_x_mag.png
      $tileInfo = explode('_', $fileName);//[img_Name, tile_y, tile_x, mag.png]
      $tileInfo[count($tileInfo) - 1] = explode('.', $tileInfo[count($tileInfo) - 1])[0];//[img_Name, tile_y, tile_x, magj]
      
      $gen = $fileInfo[count($fileInfo) - 2];//0
      $id = $fileInfo[count($fileInfo) - 3];//imgName
      
      $orgBmp = ROOT.DS.APP_DIR.'/tmp/files/image/'.$id.'/'.$gen.'/raw/'.$id.'.bmp';// /app/tmp/files/image/imgName/0/raw/imgName.bmp
      $this->layout = false;
      $this->render(false);
      $imgFile = COMMON_FILE_PATH . $filepath;// /app/tmp/files/image/imgName/0/imgName_tile_y_tile_x_mag.png
      if (!file_exists($imgFile)){
          $pi = null;
          $pipeDir = PIPE_ROOT_DIR.$id;// /Flute/pipe/imgName
          if (!file_exists($pipeDir)){
              mkdir($pipeDir, 0777, true);
          }
          $inPipe = $pipeDir.'/input';// /Flute/pipe/imgName/input
          if (!file_exists($pipeDir.'/input')){
              posix_mkfifo($inPipe, '0500');//双方向パイプ、
              $exe = ROOT.DS.'bin/core_app '. "$inPipe" . ' ' ." > /dev/null &";
              $pi = fopen($inPipe, 'w+');
              exec($exe);
              fwrite($pi, "load_bmp ".$orgBmp."\n");//画像のロード命令
              fflush($pi);
          } else {
              $pi = fopen($inPipe, 'w+');
          }
          $tilePipeDir = PIPE_ROOT_DIR.$id.'/'.$gen;// /Flute/pipe/imgName/0
          if (!file_exists($tilePipeDir)){
              mkdir($tilePipeDir, 0777, true);
          }
          $tilePipe = PIPE_ROOT_DIR.$id.'/'.$gen.'/'.$id.'_'.$tileInfo[1].'_'.$tileInfo[2].'_'.$tileInfo[3];//各タイルのパイプの作成
          if (file_exists($tilePipe)){
              unlink($tilePipe);//すでにあるパイプは削除
          }
          posix_mkfifo($tilePipe, '0500');//双方向パイプ、
          $rspPipe = fopen($tilePipe, 'w+');
          $cmd = "output_tile ".ROOT.DS.APP_DIR.'/tmp/files/image/'.$id.'/'.$gen.'/'.$tileInfo[0]." ".$tileInfo[1]." ".$tileInfo[2]." ".$tileInfo[3]." ". $tilePipe ."\n";//画像の表示命令
          
            error_log(print_r(file_get_contents($rspPipe),true),"3","/Flute/pipe/debug.log"); 
	  while (1){
            fwrite($pi, $cmd);//inputパイプに表示命令の書き込み
            fflush($pi);//連続書き込みのために、フラッシュ
            $r = fgets($rspPipe);//tilePipeからの命令を読み取り
	    if ($r == "done\n"){
              break;
            }
            if ($r == "err\n"){
              break;
            }
            if ($r == "retry\n"){
              ;
            }
            usleep(300000);
          }
          
          fclose($pi);
          fclose($rspPipe);
          unlink($tilePipe);
      }
      $finfo = new finfo(FILEINFO_MIME_TYPE);
      $mimeType = $finfo->file($imgFile);//画像ファイルの種類を取得
      header('Content-type: ' . $mimeType . '; charset=UTF-8');//種類を伝える
      readfile($imgFile);//画像ファイルの読み込み
  }
}
