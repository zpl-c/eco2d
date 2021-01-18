
Vue.component('play-button', {
  props: ["state", "icon"],
  methods: {
    clicked: function() {
      this.$emit('click', {event: this.icon});
    },
    css() {
      let result = "play-button";
      if (this.state == this.icon) {
        result += " player-active";
      }
      return result;
    }
  },
  template: `
    <div :class="css()">
      <img :src="'images/' + icon + '.png'" v-on:click="clicked">
    </div>
    `
});

Vue.component('play-controls', {
  mounted: function() {
    this.request.startRequesting("entity/flecs/core/World");
  },
  beforeDestroy: function() {
    this.stopRequesting();
  },    
  data: function() {
    return {
      state: undefined,
      request: new PeriodicRequest(100, this.on_recv_world, this.on_recv_err),
      data: undefined
    }
  },  
  methods: {
    play: function(e) {
      this.state = e.event;
      app.put("player/" + e.event, (msg) => { });
    },
    on_recv_world(data) {
      this.data = data;
      if (!this.state) {
        const state = this.data.data["flecs.player.Player"];
        if (state.state == "EcsPlayerPlay") {
          this.state = "play";
        } else if (state.state == "EcsPlayerStop") {
          this.state = "stop";
        } else if (state.state == "EcsPlayerPause") {
          this.state = "pause";
        }
      }
    },
    on_recv_err() {
      this.state = undefined;
    } 
  },
  template: `
    <div class="play-controls">
      <div class="play-buttons">
        <play-button icon="stop" :state="state" v-on:click="play"></play-button>
        <play-button icon="play" :state="state" v-on:click="play"></play-button>
        <play-button icon="pause" :state="state" v-on:click="play"></play-button>
      </div>
      <perf-summary :data="data"></perf-summary>
    </div>
    `
  });
