<?php
/*
 * config.csvを読み込む。
 *
 * @return config.csvの連想配列
 */

require_once("vendor/autoload.php");
use DiDom\Document;

if(!isset($view)) $view = getCurrentView(); // view名決定。デフォルトはindex
$config = loadConfigCsv(); // config.csvを読み込む。
$seo = loadSeoCsv(); // seo.csvを読み込む。
$link = loadLinkCsv(); // link.csvを読み込む。
$meta = loadMetaCsv(); // meta.csvを読み込む。

function loadConfigCsv() {
  $csv = array();
  $handle = fopen(__DIR__ ."/anima/config/config.csv", "r");
  if (!$handle) exit;

  fgetcsv($handle); // タイトル行をスキップ
  while ($data = fgetcsv($handle)) $csv += array($data[0] => array("isuse" => $data[1] == "TRUE", "value" => $data[2]));

  fclose($handle);
  return $csv;
}

function loadViewIncfile($dom) {
  global $view;
  global $config;

  $filename = 'anima/include/'.$view.'.inc';
  if (file_exists($filename))  require_once($filename);

  return;
}

/*
 * {$view}.htmlの存在をチェックし、ファイル名を返す
 *
 * @return $viewfile
 */
function getViewFileName() {
  global $view;
  if (file_exists(__DIR__ ."/anima/view/".$view.".html")) {
    $viewfile = $view;
    return $viewfile;

  } elseif(file_exists(__DIR__ ."/anima/view/".$view."-sp.html")) {
    $viewfile = $view."-sp";
    return $viewfile;

  } elseif(file_exists(__DIR__ ."/anima/view/".$view."-all-breakpoints.html")) {
    $viewfile = $view."-all-breakpoints";
    return $viewfile;

  } elseif(file_exists(__DIR__ ."/anima/view/".$view."-sp-all-breakpoints.html")) {
    $viewfile = $view."-sp-all-breakpoints";
    return $viewfile;

  }
}

/*
 * seotext.csvを読み込む。
 *
 * @return seotext.csvの連想配列
 */
function loadSeoCsv() {
  $csv = array();
  $handle = fopen(__DIR__ ."/anima/config/seo.csv", "r");
  if (!$handle) exit;

  fgetcsv($handle); // タイトル行をスキップ
  while ($data = fgetcsv($handle)) {
    if ($data[0]) $csv[$data[0]] = $data[1];
  }

  fclose($handle);
  return $csv;
}

/*
 * link.csvを読み込む。
 *
 * @return link.csvの連想配列
 */
function loadLinkCsv() {
  $csv = array();
  $handle = fopen(__DIR__ ."/anima/config/link.csv", "r");
  if (!$handle) exit;

  fgetcsv($handle); // タイトル行をスキップ
  while ($data = fgetcsv($handle)) $csv[$data[0]] = array("link" => $data[1], "target" => $data[2]);

  fclose($handle);
  return $csv;
}

/*
 * meta.csvを読み込む。
 * ページURLが現在のviewのもののみ。
 *
 * @return meta.csvの連想配列
 */
function loadMetaCsv() {
  global $view;
  $csv = array();
  $handle = fopen(__DIR__ ."/anima/config/meta.csv", "r");
  if (!$handle) exit;

  $target = $view;
  fgetcsv($handle); // タイトル行をスキップ
  while ($data = fgetcsv($handle)) {
    if ($data[0] == $target) {
      $csv["title"] = $data[1];
      $csv["description"] = $data[2];
      $csv["keywords"] = $data[3];
      break;
    }
  }

  fclose($handle);
  return $csv;
}

/*
 * <body>直下に<div id="loading"></div>を追加する。
 *
 * @param $dom ドキュメントのDOM
 */
function appendLoading($dom) {
  global $theme_uri;
  $dom->first("body")->style()->setProperty('opacity', '0');
  $loading = $dom->createElement("div", null, ["id" => "loading"]);
  $innerHtml = '
  <div class="loading-img-wrap"><img class="loading-img bg" src="'.$theme_uri.'/anima/images/loading.png"
  srcset="'.$theme_uri.'/anima/images/loading.png 1x, '.$theme_uri.'/anima/images/loading@2x.png 2x">
    <div class="progress">
      <div><img class="loading-img" src="'.$theme_uri.'/anima/images/loading.png"
          srcset="'.$theme_uri.'/anima/images/loading.png 1x, '.$theme_uri.'/anima/images/loading@2x.png 2x"></div>
    </div>
  </div>
  ';
  $loading->setInnerHtml($innerHtml);
  $ref = $dom->first("body")->child(0);
  $dom->first("body")->insertBefore($loading, $ref);
}

/*
 * <div class="screen">直下に<div class="design-check"></div>を追加する。
 *
 * @param $dom ドキュメントのDOM
 */
function appendDesignCheck($dom) {
  global $theme_uri;
  global $view;
  global $config;
  $spbreakpoint = $config["spbreakpoint"];
  $sp = htmlspecialchars($_COOKIE['windowWidth']) < $spbreakpoint["value"] ? '-sp' : '';
  $designcheck = $dom->createElement("div", null, ["id" => "designcheck"]);
  $designcheck->style()->setProperty('background-image', "url('{$theme_uri}/anima/images/designcheck/{$view}{$sp}.png')");
  $designcheckBtn = $dom->createElement("div", null, ["id" => "designcheck-btn"]);
  $ref = $dom->first(".screen")->child(0);
  $dom->first(".screen")->insertBefore($designcheck, $ref);
  $dom->first(".screen")->insertBefore($designcheckBtn, $ref);
}

/*
 * 要素は「src」属性を持っていなければならない。
 *
 * @param $elemsWithSrc 処理する要素の配列
 */
function appendViewPrefixToSrc($elemsWithSrc)
{
  global $theme_uri;
  foreach ($elemsWithSrc as $elem) $elem->attr('src', "{$theme_uri}/anima/view/{$elem->attr('src')}");
}

/*
 * すべての要素の「href値に「view」を追加する。
 * 要素は「href」属性を持っていなければならない。
 *
 * @param $elmsWithHref 処理する要素の配列
 */
function appendViewPrefixToHref($elemsWithHref)
{
  global $theme_uri;
  foreach ($elemsWithHref as $elem) $elem->attr("href", "{$theme_uri}/anima/view/{$elem->attr('href')}");
}

/*
 * 使用しない「.screen」を削除し、content-pc・content-spを追加する。
 *
 * @param $dom ドキュメントのDOM
 */
function removeUnusedScreen($dom)
{
  global $config;
  $spbreakpoint = $config["spbreakpoint"];
  if (!$spbreakpoint["isuse"]) return;

  $pcWraps = $dom->find('[class*="pc-wrap"]');
  if(count($pcWraps) > 0) {
    $currentClasses = $pcWraps[0]->parent()->getAttribute("class");
    $pcWraps[0]->parent()->setAttribute("class", "$currentClasses content-pc");
  }

  $spWraps = $dom->find('[class*="sp-wrap"]');
  if(count($spWraps) > 0) {
    $currentClasses = $spWraps[0]->parent()->getAttribute("class");
    $spWraps[0]->parent()->setAttribute("class", "$currentClasses content-sp");
  }

  $target = htmlspecialchars($_COOKIE['windowWidth']) < $spbreakpoint["value"] ? '[class*="pc-wrap"]' : '[class*="sp-wrap"]';
  $dom->first($target)->parent()->parent()->remove();
}

/*
 * view内の無用なタグを削除する。
 *
 * @param $dom ドキュメントのDOM
 * @return string 無用なタグを削除したHTML
 */
function output($dom) {
  global $config;
  $output = $dom->format()->html();

  // bodyタグの直後、body終了タグの直前に挿入
  $bodyTop = file_get_contents("./anima/config/{$config["insert_body_top"]["value"]}", true);
  $bodyBottom = file_get_contents("./anima/config/{$config["insert_body_bottom"]["value"]}", true);
  $output = str_replace("</body>",$bodyBottom."</body>", $output);
  preg_match('/<body[^>]*>/', $output, $bodytag);
  $output = str_replace($bodytag[0],$bodytag[0].$bodyTop, $output); 

  // 整形
  $output = substr($output, strlen("<!DOCTYPE html>\n<html>\n  <head>\n"));

  // 不要な要素を削除
  $output = str_replace([
    '<!--<meta name=description content="This site was generated with Anima. www.animaapp.com"/>-->',
    '<!-- <link rel="shortcut icon" type=image/png href="https://animaproject.s3.amazonaws.com/home/favicon.png" /> -->',
    '<link rel="shortcut icon" type="image/png" href="https://animaproject.s3.amazonaws.com/home/favicon.png">'
  ], "", $output);

  return $output;
}

/*
 * 指定された要素をsection要素に変換する。
 *
 * @param $elems 対象要素の配列
 */
function toSection($elems) {
  foreach ($elems as $elem) {
    $section = new Didom\Element('section', null, $elem->attributes());
    $section->setInnerHtml($elem->innerHtml());
    $elem->replace($section);
  }
}

/*
 * p要素をdiv要素に変換する。
 *
 * @param $ps 対象pの配列
 */
function changePToDiv($ps) {
  foreach ($ps as $div) {
    $elem = new Didom\Element('div', null, $div->attributes());
    $elem->setInnerHtml($div->innerHtml());

    $div->replace($elem);
  }
}

/*
 * h1要素をdiv要素に変換する。
 *
 * @param $h1s 対象h1の配列
 */
function changeH1ToDiv($h1s) {
  foreach ($h1s as $div) {
    $elem = new Didom\Element('div', null, $div->attributes());
    $elem->setInnerHtml($div->innerHtml());

    $div->replace($elem);
  }
}

/*
 * 指定されたdiv要素をクラスで指定された要素に変換する。
 * 変換できるのはh1〜6およびpのみ。
 *
 * @param $divs 対象divの配列
 */
function changeDivToHOrPSpecifiedByClass($divs) {
  foreach ($divs as $div) {
    $elem = new Didom\Element(getHOrPFromClass($div), null, $div->attributes());
    $elem->setInnerHtml(getStrDivOfChildToSpan($div));

    $div->replace($elem);
  }
}

/*
 * <div>内の<div>を<span>に変換した文字列を得る。
 *
 * @param $div 対象div
 * @return string <div>を<span>に変換した文字列
 */
function getStrDivOfChildToSpan($div) {
  $res = $div->innerHtml();
  $res = str_replace("<div", "<span", $res);
  $res = str_replace("</div>", "</span>", $res);
  return $res;
}

/*
 * 指定された要素のIDを設定する。
 * IDは要素のクラス文字列「-id_foobar_-」で指定されているものとする。
 * 「foobar」がIDとなる。
 *
 * 例:
 *   <div class="button original-button-id_foobar_-99999">
 *   <div class="button id_foobar_">
 *
 * @param $elems 対象要素の配列
 */
function assignId($elems) {
  foreach ($elems as $elem) {
    // 「-id_foobar_」から「foobar」を得る。
    $classes = $elem->classes()->getAll();
    $class = (($c = preg_grep("/-id_[^_]+_/", $classes)) ? $c : preg_grep("/^id_[^_]+_/", $classes))[0];

    preg_match("/id_([^_]+)_/", $class, $a);
    $elem->attr("id", $a[1]); // 要素のIDを設定する。「-id_foobar_」が複数ある場合は最初のものとする。
  }
}

/*
 * 指定されたimg要素をクラスで指定された要素で囲う
 * 囲えるのはh1〜6およびpのみ。
 *
 * @param $imgs 対象imgの配列
 */
function wrapImgWithHOrPSpecifiedByClass($imgs) {
  foreach ($imgs as $img) {
    $elem = new Didom\Element(getHOrPFromClass($img));
    $elem->appendChild($img);
    $img->replace($elem);
  }
}

/*
 * 指定された要素のクラスで指定されたタグ名を得る。
 * 指定できるのはh1〜6およびpのみ。
 *
 * @param $elm 対象要素
 */
function getHOrPFromClass($elem) {
  // 要素のクラスから/^(h[1-6]|p)-txt-/を持つクラス文字列を得る。複数ある場合は最初のものとする。
  $class = preg_grep("/^(h[1-6]|p)-/", $elem->classes()->getAll())[0];

  preg_match("/^(h[1-6]|p)-/", $class, $a);
  return $a[1];
}

/*
 * <img>のalt属性を空にする
 */
function removeAltToImg($imgs) {
  foreach ($imgs as $img) {
    $img->attr("alt", '');
  }
}

/*
 * 指定された<img>に、クラスで指定されたalt属性を持たせる。
 * alt文字列はCSVファイルに指定されているものとする。
 * @param $imgs 対象imgの配列
 */
function appendAltToImgFromClass($imgs) {
  global $seo;

  foreach ($imgs as $img) {
    // <img>のクラスから/txt-.+/を持つクラス文字列を得る。複数ある場合は最初のものとする。
    $classes = $img->classes()->getAll();
    $matched_classes = preg_grep("/(\b|-)seo-.+/", $classes);
    
    if (!empty($matched_classes)) {
      $class = reset($matched_classes);
      preg_match("/(\\b|-)seo-[^\-]+/", $class, $a);
      $key = trim($a[0], "-");

      $img->attr("alt", $seo[$key]);
    }
  }
}

/*
 * 指定されたタグ配下に、テキストを含む隠しspan要素を挿入する。
 * @param $tags 対象tagの配列
 */
function appendSeotxtToTagFromClass($tags, $dom) {
  global $seo;

  foreach ($tags as $tag) {
    $classes = $tag->classes()->getAll();
    $matched_classes = preg_grep("/(\b|-)seo-.+/", $classes);
    
    if (!empty($matched_classes)) {
      $class = reset($matched_classes);
      preg_match("/(\\b|-)seo-[^\-]+/", $class, $a);
      $key = trim($a[0], "-");

      $span = $dom->createElement('span', $seo[$key]);
      $span->setAttribute('class', 'hidden-txt');
      $tag->appendChild($span);
    }
  }
}


/*
 * 指定された要素を<a>で囲む。
 * リンク先はCSVファイルに指定されているものとする。
 * @param $elms 対象要素の配列
 */
function wrapA($elms) {
  global $link;

  foreach ($elms as $elm) {
    // 要素のクラスから/link-\d\d\d\d/を持つクラス文字列を得る。複数ある場合は最初のものとする。
    $class = preg_grep("/(\b|-)link-\w+/", $elm->classes()->getAll())[0];

    preg_match("/(\b|-)(link-\w+)/", $class, $a);
    if (!$l = $link[$a[2]]) continue;

    $a = new Didom\Element("a", null, ["href" => $l["link"]]);
    $a->appendChild($elm);
    if ($l["target"]) $a->attr("target", $l["target"]);
    $elm->replace($a);
  }
}

/*
 * 指定された<img>のsrcをdata-originalにする。
 *
 * @param $img 処理する<img>の配列
 */
function changeImgSrcToDataOriginal($imgs) {
  global $theme_uri;
  foreach ($imgs as $img) {
    $img->attr("data-original", $img->attr("src"));
    $img->attr("src", $theme_uri."/anima/images/dummy.png");
  }
}

/*
 * 指定された<img>のdata-originalをsrcにする。
 *
 * @param $img 処理する<img>の配列
 */
function changeImgDataOriginalToSrc($imgs) {
  foreach ($imgs as $img) {
    $img->attr("src", $img->attr("data-original"));
    $img->removeAttribute("data-original");
  }
}

/*
 * 現在のURLを得る。
 * 但し末尾の#ハッシュ値は消す。
 * https://blog.codecamp.jp/php-request-url
 *
 * @return string 現在のURL
 */
function getCurrentUrlWithoutHash() {
  preg_match("/^([^#]+)#?/", $_SERVER["REQUEST_URI"], $a);
  $path = $a[1];
  return (empty($_SERVER['HTTPS']) ? 'http://' : 'https://') . $_SERVER['HTTP_HOST'] . $path;
}

/*
 * view名を得る。
 * デフォルトはindex。
 *
 * @return string view名
 */
function getCurrentView() {
  if (isset($_GET['view'])) {
    $res = htmlspecialchars($_GET['view']);
    return $res;
  }
  if (preg_match("/(\w+)\.html/", htmlspecialchars($_SERVER["REQUEST_URI"]), $a)) return $a[1];
  return "index";
}


/*
 * Animate.css用処理
 * 子要素をdiv.wrap要素で囲う
 *
 * @param $elms 対象要素の配列
 */
function wrapChildDivForAnimate($elms) {
  foreach ($elms as $elm) {
    $attributes = ['class' => 'wrap'];
    $divChild = new Didom\Element("div", null, $attributes);
    $divChild->appendChild($elm->children());

    $attributes = ['class' => $elm->first('*')->getAttribute('class')];
    $div = new Didom\Element("div", null, $attributes);
    $div->appendChild($divChild);
    $elm->replace($div);
  }
}


/*
 * Animate.css用処理
 * 子要素をspan.wrap要素で囲う
 *
 * @param $elms 対象要素の配列
 */
function wrapChildspanForAnimate($elms) {
  foreach ($elms as $elm) {
    $attributes = ['class' => 'wrap'];
    $spanChild = new Didom\Element("span", null, $attributes);
    $spanChild->appendChild($elm->children());

    $attributes = ['class' => $elm->first('*')->getAttribute('class')];
    $span = new Didom\Element("span", null, $attributes);
    $span->appendChild($spanChild);
    $elm->replace($span);
  }
}


/*
 * replace-viewimg/img/に画像が存在する場合、changeImgSrcToReplaceImg()で各imgのsrc属性を置き換える
 *
 * @param $img 処理する<img>の配列
 */
function replaceImgsSrc($dom) {
  global $theme_uri;
  $path = __DIR__ .'/anima/images/replace-viewimg/img/';
  $replacePath = $theme_uri.'/anima/images/replace-viewimg/img/';
  $replaceImgs = glob($path.'*');
  foreach ($replaceImgs as $src) {
    $src = str_replace($path, '/', $src);
    changeImgSrcToReplaceImg($dom->find('img[src*="'.$src.'"]'), $src, $replacePath);
  }
}


/*
 * 指定された<img>のsrcをreplace-viewimg/img/の画像に置き換える
 *
 * @param $img 処理する<img>の配列
 */
function changeImgSrcToReplaceImg($imgs, $src, $path) {
  foreach ($imgs as $img) {
    $img->attr("src", $path.$src);
  }
}


/*
 * replace-viewcss/にCSSが存在する場合、changeCssHrefToReplaceCss()で各linkタグのhref属性を置き換える
 *
 * @param $img 処理する<img>の配列
 */
function replaceCssHref($dom) {
  global $theme_uri;
  $path = __DIR__ .'/anima/css/replace-viewcss/';
  $replacePath = $theme_uri.'/anima/css/replace-viewcss/';
  $replaceCss = glob($path.'*');
  foreach ($replaceCss as $css) {
    $file = str_replace($path, '', $css);
    changeCssHrefToReplaceCss($dom->find('link[href*="'.$file.'"]'), $file, $theme_uri);
  }
}

/*
 * 指定された<link>のhrefをreplace-viewcss/のCSSに置き換える
 *
 * @param $css 処理する<link>の配列
 */
function changeCssHrefToReplaceCss($csses, $file, $path) {
  foreach ($csses as $css) {
    $css->attr("href", $path.'/anima/css/replace-viewcss/'.$file);
  }
}


/*
 * Barba.js用の属性を追加する
 * 
 * @param $dom ドキュメントのDOM
 */
function addBarba($dom) {

  // bodyにdata-barba="wrapper"を追加
  $dom->first("body")->attr("data-barba", "wrapper");

  // .screenの子要素にdata-barba="container"とdata-barba-namespaceを追加
  global $view; 
  $containers = $dom->find(".container-center-horizontal");
  foreach ($containers as $container) {
    $container->attr("data-barba", "container");
    $container->attr("data-barba-namespace", $view); 
  }

  appendBarbaInitScript($dom);
  moveLinkToTopOfContainer($dom);

}

/*
 * Barba.jsの初期化スクリプトをbody終了タグの直前に追加する
 *
 * @param $dom ドキュメントのDOM
 */  
function appendBarbaInitScript($dom) {

  $script = $dom->createElement("script");
  $script->attr("src", "https://unpkg.com/@barba/core");

  $script2 = $dom->createElement("script");

  $innerScript = <<<EOD
barba.init({
  transitions: [{
    name: 'default-transition',
    leave() {
      document.getElementById('loading').style.opacity = '1';
      document.getElementById('loading').style.transition = 'all 0s ease';
      window.scrollTo(0, 0);
    },
    enter() {
      setTimeout(function () {
        var appendScript = document.getElementById('appendScript');
        if (appendScript) {
          var scriptSrc = appendScript.getAttribute('src');
          
          var scriptElement = document.createElement('script');
          scriptElement.src = scriptSrc;
          
          var container = document.querySelector('.container-center-horizontal');
          container.appendChild(scriptElement);
        }
      }, 10);

      $('[class*="firstview"] [class*="anim-"]').css('display', 'none');

      setTimeout(function () {
        var instShowOnScroll = new ShowOnScroll();
        instShowOnScroll.init();
        $('[class*="firstview"] [class*="anim-"]').css('display', '').addClass("anim-showed");
        document.getElementById('loading').style.removeProperty('opacity');
        document.getElementById('loading').style.transition = 'all .2s ease';

        let checkExistDummyImgInterval = null; 
        function checkExistDummyImg() {
          const dummyImgs = document.querySelectorAll('img[src*="dummy.png"]');
          if (dummyImgs.length > 0) {
            clearInterval(checkExistDummyImgInterval); 
            imgDelayLoad();
            checkExistDummyImgInterval = setInterval(checkExistDummyImg, 100);
          } // else {
            //console.log('No dummy image found. Continuing search...'); 
          //}
        }
        function startCheckExistDummyImg() {
          checkExistDummyImgInterval = setInterval(checkExistDummyImg, 100);
          setTimeout(() => {
            clearInterval(checkExistDummyImgInterval);
          }, 10000); 
        }
        startCheckExistDummyImg();

      }, 250);
    }
  }]
});  
EOD;

  $script2->setInnerHtml($innerScript);

  $body = $dom->find("body")[0];
  $body->appendChild($script);
  $body->appendChild($script2);

}

/*
 * link要素を.container-center-horizontalの先頭に移動する
 */  
function moveLinkToTopOfContainer($dom) {
  $container = $dom->find('.container-center-horizontal')[0];
  $link = $dom->find('link[rel="stylesheet"]')[0];
  
  $container->prependChild($link);
  $link->remove(); 

  global $view, $theme_uri;

  // viewのCSSがあれば追加
  if (file_exists(__DIR__ ."/anima/css/{$view}.css")) {
    $css = $dom->createElement("link");
    $css->attr("rel", "stylesheet");
    $css->attr("type", "text/css");
    $css->attr("href", $theme_uri."/anima/css/{$view}.css");
    $container->prependChild($css);
  }

  // viewのJSがあれば追加
  if (file_exists(__DIR__ ."/anima/js/{$view}.js")) {  
    $js = $dom->createElement("script");
    $js->attr("src", $theme_uri."/anima/js/{$view}.js");
    $js->attr("id", "appendScript");
    $js->attr("defer", "defer");
    $container->appendChild($js);
  }
}