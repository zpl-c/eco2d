
class PeriodicRequest {
  constructor(interval, callback, err_callback) {
    this.error = false;
    this.timer = undefined;
    this.result = undefined;
    this.interval = interval;
    this.callback = callback;
    this.err_callback = err_callback;
  }

  request(url) {
    app.get(url, (msg) => {
      this.result = msg;
      this.error = false;
      this.callback(msg);
    }, (Http) => {
      if (!Http.status || Http.status >= 400) {
        this.err_callback(Http.status);
        this.error = true;
      }
    });
  }

  stopRequesting() {
    this.result = undefined;
    clearInterval(this.timer);
  }

  startRequesting(url) {
    this.stopRequesting();
    this.timer = window.setInterval(function() {
      this.request(url);
    }.bind(this), this.interval);
  }
}
