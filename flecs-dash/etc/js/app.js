
Vue.config.devtools = true;

var app = new Vue({
  el: '#app',
  methods: {
    // Request
    request(method, url, onmsg, onloadend) {
      const Http = new XMLHttpRequest();

      Http.open(method, "http://" + url);
      if (onloadend) {
        Http.onloadend = function() { onloadend(Http); };
      }
      Http.send();
      Http.onreadystatechange = (e)=>{
        if (Http.readyState == 4) {
          if (Http.responseText && Http.responseText.length) {
            if (onmsg) {
              onmsg(JSON.parse(Http.responseText));
            }
          }
        }
      }
    },

    // Request to own server
    request_self(method, url, onmsg, onloadend) {
      this.request(method, this.host + "/" + url, onmsg, onloadend);
    },

    set_host(host) {
      this.host = host;
      this.init();
    },

    get(url, onmsg, onloadend) {
      this.request_self("GET", url, onmsg, onloadend);
    },

    put(url, onmsg, onloadend) {
      this.request_self("PUT", url, onmsg, onloadend);
    },  
    
    post(url, onmsg, onloadend) {
      this.request_self("POST", url, onmsg, onloadend);
    },      

    delete(url, onmsg, onloadend) {
      this.request_self("DELETE", url, onmsg, onloadend);
    },
    
    path_to_url(path) {
      return path.replace(/\./g, "/");
    },

    // Initialize application, send initial request for id of self
    init() {
      this.get("this", (msg) => {
        this.server_id = msg.server_id;
        this.get("scope/" + this.path_to_url(this.server_id) + "?include=flecs.dash.App", (msg) => {
          this.apps = msg;
        });
      });
    },

    // Anything that needs to appen periodically
    refresh() { },

    // App select event selects a different application
    app_select(event) {
      this.app_load(event.name);
    },

    import(url) {
      var app = document.getElementById(url);
      if (!app) {
        app = document.createElement("script");
        app.setAttribute("id", url);
        app.setAttribute("src", url);
        document.body.appendChild(app);
        
        return false
      } else {
        return true;
      }
    },

    component_loaded(id) {
      return Vue.options.components[id] != undefined;
    },

    // Load app resources
    app_load(app_id) {
      if (this.import("apps/" + app_id + "/" + app_id + ".js")) {
        this.app_loaded(app_id);
      }
    },

    // App is loaded
    app_loaded(app_id, dependencies) {
      var dependencies_resolved = true;

      // Load dependent components if any
      if (dependencies) {
        dependencies.forEach(d => {
          if (!this.component_loaded(d.name)) {
            app.import(d.url);
            dependencies_resolved = false;
          }
        });
      }

      // If all dependencies are resolved, just load the app
      if (dependencies_resolved) {
        this.app = app_id;
      } else {
        // If not all dependencies are loaded, try again in a little bit
        window.setTimeout(function() {
          this.app_loaded(app_id, dependencies);
        }.bind(this), 32);
      }
    }
  },
  data: {
    host: window.location.host,
    app: "",
    apps: [],
    server_id: ""
  }
});

window.onload = function() {
  app.init();
  window.setInterval(app.refresh, 1000);
}
