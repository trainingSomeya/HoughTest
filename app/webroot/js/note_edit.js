$(function(){
  var w_width = $('#draw_window').width();//ウィンドウサイズ？
  var w_height = $('#draw_window').height();
  var cam_x = 0;//キャンパス上の位置？
  var cam_y = 0;
  var mag = $('#mag_selector').val(); /* 表示倍率 */
  var tile_w = 256; //タイルのサイズ？
  var tile_h = 256;
  
  var mx = 0;//マウス位置？
  var my = 0;
  var drag = false;
  var fileNameData;
  
  $('#mag_selector').change(function(){
    mag = $('#mag_selector').val();
    $('#draw_window').html('');
    $('#pre_load_area').html('');
    draw();
  });
<<<<<<< HEAD
 //表示のチェック用 
	function sleep(milliseconds) {
		return new Promise(resolve => setTimeout(resolve, milliseconds));
	}

  function load_img(url, id){
=======
  
  function load_img(url, id){//??
>>>>>>> 770a31e902234a78d6efa76ffc3515d6113bed45
    //imgPreloaderオブジェクトを作る、名前は任意
    var imgPreloader=new Image();
    //onloadイベントハンドラ追加
    imgPreloader.onload= function() {
      //ロード完了で画像を表示
      $('#'+id).attr({'src':url});
    }
    //重要、最後に書く
    imgPreloader.src=url;
  }
	
  function draw() {
    var ost_x = ((cam_x % tile_w) + tile_w) % tile_w;//端数?
    var ost_y = ((cam_y % tile_h) + tile_h) % tile_h;
    var tile_x = Math.floor(((w_width + ost_x) + tile_w - 1) / tile_w);//ウィンドウに詰めているタイルの数?
    var tile_y = Math.floor(((w_height + ost_y) + tile_h - 1) / tile_h);
    var x;//番目?
    var y;
    var t_ost_x = Math.floor(cam_x / tile_w);//表示されている範囲で現在位置が何番目のタイルか?
    var t_ost_y = Math.floor(cam_y / tile_h);
    var pre_load_size = 2;//予約の範囲?
    var generated_meta_info = [];//タイルタグ生成のhtmlを格納？
    
    var imgName = $('#file_name_data').val();
    var id_prefix = 'tile';
    var preload_id_prefix = 'preload_tile';
    var file_prefix = '/read/image/' + imgName + '/0' + '/' + imgName;//画像ファイルへのurlの一部?

    var lft_x = t_ost_x - 1;//表示画面外の左のタイル番号,使ってない？
    var lft_y = t_ost_y - 1;
    var rght_x = tile_x + t_ost_x;//表示画面外の右のタイル番号,使ってない？
        var h = tile_h;
        if (x * tile_w - ost_x < 0){//左端の部分のサイズの調整？
          w = tile_w - ost_x;
        }
        if (y * tile_h - ost_y < 0){//上端の部分のサイズの調整？
          h = tile_h - ost_y;
        }
        if (x * tile_w + w - ost_x > w_width){//右端の部分のサイズの調整？
          w = w_width - x * tile_w + ost_x;
        }
        if (y * tile_h + h - ost_y > w_height){//下端の部分のサイズの調整？
          h = w_height - y * tile_h + ost_y;
        }
        if (w < 0 || h < 0){
          continue;
        }
        var tile_pos_x = (tile_w*x-ost_x);//敷き詰めるタイルの左端の位置(後で0番目は０に成る)？
        var tile_pos_y = (tile_h*y-ost_y);//敷き詰めるタイルの上端の位置（後で０番目は０に成る）？
        htm += gen_img_tag(id_prefix, (y + t_ost_y), (x + t_ost_x), preload_id_prefix, w, h, tile_pos_y, tile_pos_x);
        generated_meta_info.push({prefix:id_prefix, y:(y + t_ost_y), x: (x + t_ost_x)});
      }
    }
    $('#draw_window').html(htm);

    htm = '';
    for (p = 0 ; p < pre_load_size ; p++){//画面外のタイルの予約？？
      for (x = 0 ; x < tile_x ; x++){
        var pre_y = t_ost_y -1 - p;
        var pre_x = x + t_ost_x;
        htm += gen_img_tag(preload_id_prefix, pre_y, pre_x);
        generated_meta_info.push({prefix:preload_id_prefix, y:pre_y, x: pre_x});
        pre_y = tile_y + t_ost_y + p;
        pre_x = x + t_ost_x;
        htm += gen_img_tag(preload_id_prefix, pre_y, pre_x);
        generated_meta_info.push({prefix:preload_id_prefix, y:pre_y, x: pre_x});
      }
      for (y = - pre_load_size ; y < tile_y + pre_load_size ; y++){
        var pre_y = y + t_ost_y;
        var pre_x = t_ost_x -1 - p;
        htm += gen_img_tag(preload_id_prefix, pre_y, pre_x);
        generated_meta_info.push({prefix:preload_id_prefix, y:pre_y, x: pre_x});
        pre_y = y + t_ost_y;
        pre_x = tile_x + t_ost_x + p;
        htm += gen_img_tag(preload_id_prefix, pre_y, pre_x);
        generated_meta_info.push({prefix:preload_id_prefix, y:pre_y, x: pre_x});
      }
    }
    $('#pre_load_area').html(htm);
<<<<<<< HEAD
   
	  //表示のチェック用
	  (async () => {
		  for (let i of generated_meta_info){
			  await sleep(3000);
			  combine_fpath_id(i.prefix,file_prefix, i.y, i.x);
		  }
	  })();
    /*generated_meta_info.forEach(async function (o){
     combine_fpath_id(o.prefix, file_prefix, o.y , o.x);
    });*/
=======
    
    generated_meta_info.forEach(function (o){//タイルをつなげて表示?？
      combine_fpath_id(o.prefix, file_prefix, o.y , o.x);
    });
>>>>>>> 770a31e902234a78d6efa76ffc3515d6113bed45
  };
	//タイルの位置を決めるタグの作成？
  function gen_img_tag(id_prefix, tile_y, tile_x, preload_id_prefix = '', width = 0, height = 0, pos_y = 0, pos_x = 0){
    if ((tile_x < 0 || tile_y < 0) && width == 0 && height == 0 ){
       return '';
    }
    var htm = '';
    var id = id_prefix + '_' + tile_y + '_' + tile_x;
    var src = '';
	  //タイルidの記述が存在しているかつ、srcが定義済みであるかつ、タイルidタグのsrcの要素が存在している?
    if ($('#' + id).length && $('#' + id).attr('src') != undefined && $('#' + id).attr('src').length){
      src = $('#' + id).attr('src');//指定タイルのsrcを取得?
    }
	  //
    if (width != 0 && height != 0){
      var preload_id = preload_id_prefix + '_' + tile_y + '_' + tile_x;//予約タイルのidの名前作成?
      var img_style = 'pointer-events: none;';//要素がマウスのターゲットではないことを示す?
      if (pos_x < 0){
        pos_x = 0;
        img_style += 'right:0;'//左端？
      }
      if (pos_y < 0){
        pos_y = 0;
        img_style += 'bottom:0;'//上端？
      }
      if (img_style.length){
        img_style = 'style="position:absolute;' + img_style +'"';//
      }
	    //srcが記述されていないかつ、予約タイルの記述が存在するかつ、予約タイルのsrcが定義済みであるかつ、予約タイルidタグのsrcの記述が存在している?
      if (src.length == 0 && $('#' + preload_id).length && $('#' + preload_id).attr('src') != undefined && $('#' + preload_id).attr('src').length){
        src = $('#' + preload_id).attr('src');//指定タイルのsrcを取得？
      }
      if (src.length){//srcの記述が存在しているなら
        src = ' src="' + src + '"';//srcを記述
      }
      htm += '<div style="width:' + width + 'px;height:' + height + 'px;position:absolute;left:'+ pos_x +'px;top:'+ pos_y +'px;overflow:hidden;">';//位置を指定
      htm += '<img id="' + id + '"' + img_style + src +' >\n';
      
      htm += '</div>';
    } else {
      if (src.length){
        src = ' src="' + src + '"';
      }
      htm += '<img id="' + id + '"' + src +' >\n';
    }
    return htm;
  }
	//分割した画像の表示?
  function combine_fpath_id(id_prefix, fpath_prefix, tile_y, tile_x){
    if (tile_x < 0 || tile_y < 0){
       return;
    }
    var id = id_prefix + '_' + tile_y + '_' + tile_x;
    var url = fpath_prefix + '_' + tile_y + '_' + tile_x + '_' + mag + '.png';
	  //srcが定義されているかつ、何かしらの文字列が入っている
    if (($('#' + id).attr('src')) != undefined && ($('#' + id).attr('src')).length){
      return;
    }
    load_img(url, id);
  }
	//マウスのアクション達、キャンパス上の位置の移動？
  $('#draw_window').mousedown(function (e){
    mx = e.pageX - this.offsetLeft;
    my = e.pageY - this.offsetTop;
    drag = true;
  });
  
  $('#draw_window').mousemove(function (e){
    var p_mx = mx;
    var p_my = my;
    mx = e.pageX - this.offsetLeft;
    my = e.pageY - this.offsetTop;
    if (drag){
      cam_x += p_mx - mx;
      cam_y += p_my - my;
      draw();
    }
  });
  $('#draw_window').mouseup(function (e){
    mx = e.pageX - this.offsetLeft;
    my = e.pageY - this.offsetTop;
    drag = false;
  });
  $('#draw_window').mouseleave(function (e){
    mx = 0;
    my = 0;
    drag = false;
  });
  var timer = false;
  $(window).resize(function() {
      if (timer !== false) {
          clearTimeout(timer);
      }
      timer = setTimeout(function() {
         w_width = $('#draw_window').width();
         w_height = $('#draw_window').height();
         draw();
         //console.log('width:'+w_width);
         //console.log('height:'+w_height);
          // 何らかの処理
      }, 200); // リサイズ中かどうか判定する時間、ms
  });
<<<<<<< HEAD
  draw();
});
=======
>>>>>>> 770a31e902234a78d6efa76ffc3515d6113bed45
