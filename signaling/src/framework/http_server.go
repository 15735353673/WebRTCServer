package framework

import (
	"fmt"
	"net/http"
	"signaling/src/glog"
	"strconv"
)

// 在模块加载之前自动调用
func init() {
	//将所有http请求转到entry进行处理
	http.HandleFunc("/", entry)

}

// 处理路由信息
type ActionInterface interface {
	Execute(w http.ResponseWriter, cr *ComRequest)
}

type ComRequest struct {
	R      *http.Request
	Logger *ComLog
	LogId  uint32
}

// key : URL Value:Actioninterface
var GActionRouter map[string]ActionInterface = make(map[string]ActionInterface)

func responseError(w http.ResponseWriter, r *http.Request, status int, err string) {
	w.WriteHeader(status)
	w.Write([]byte(fmt.Sprintf("%d - %s", status, err)))
}

func getRealClientIP(r *http.Request) string {
	ip := r.RemoteAddr

	if rip := r.Header.Get("X-Real-IP"); rip != "" {
		ip = rip
	} else if rip = r.Header.Get("X-Forwarded-IP"); rip != "" {
		ip = rip
	}
	return ip

}

func entry(w http.ResponseWriter, r *http.Request) {
	if "/favicon.ico" == r.URL.Path {
		w.WriteHeader(http.StatusOK)
		w.Write([]byte(""))
		return
	}
	fmt.Println("==========", r.URL.Path)
	if action, ok := GActionRouter[r.URL.Path]; ok {
		//解析
		if action != nil {
			r.ParseForm()

			//要把comlog传进去
			cr := &ComRequest{
				R:      r,
				Logger: &ComLog{},
				LogId:  GetLogId32(),
			}
			//添加通用的log字段
			cr.Logger.AddNotice("logId", strconv.Itoa(int(cr.LogId)))
			cr.Logger.AddNotice("url", r.URL.Path)
			cr.Logger.AddNotice("referer", r.Header.Get("Referer"))
			cr.Logger.AddNotice("cookie", r.Header.Get("Cookie"))
			cr.Logger.AddNotice("ua", r.Header.Get("User-Agent"))
			cr.Logger.AddNotice("clientIP", r.RemoteAddr)
			cr.Logger.AddNotice("realclientIP", getRealClientIP(r))
			//遍历一下请求参数
			for k, v := range r.Form {
				cr.Logger.AddNotice(k, v[0])
			}
			cr.Logger.TimeBegin("totalaction")
			action.Execute(w, cr)
			cr.Logger.TimeEnd("totalaction")
			cr.Logger.Infof("")
		} else {
			responseError(w, r, http.StatusInternalServerError, "Internal server error")
		}

	} else {
		responseError(w, r, http.StatusNotFound, "Not Found")
	}
}

func RegisterStaticUrl() {
	fs := http.FileServer(http.Dir(gconf.staticDir))
	http.Handle(gconf.staticPrefix, http.StripPrefix(gconf.staticPrefix, fs))
}

func StartHttp() error {
	glog.Infof("start http server on port:%d", gconf.port)
	return http.ListenAndServe(fmt.Sprintf(":%d", gconf.port), nil)
}
func StartHttps() error {
	glog.Infof("start https server on port:%d", gconf.httpsPort)
	return http.ListenAndServeTLS(fmt.Sprintf(":%d", gconf.httpsPort), gconf.httpsCert, gconf.httpsKey, nil)
}
