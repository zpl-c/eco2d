Vue.component('appcontent', {
  props: ['world', 'app'],
  render: function(h) {
    if (!this.app || this.app === "") {
      return h('div');
    } else {
      return h('div', {
        attrs: {class: "content"}
      }, [
        h('app-' + this.app, {
          props: {
            world: this.world
          }
        })
      ]);
    }
  }
});
