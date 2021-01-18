
Vue.component('entity-tree-item', {
  props: [ "entity", "current", "text", "nav_only", "" ],
  data: function() {
    return {
      clicks: 0,
      timer: null    
    }
  },
  methods: {
    nav_scope: function() {
      this.$emit("nav-scope", {entity: this.entity.name});
    },

    single_click_nav_scope: function() {
      if (this.nav_only) {
        this.nav_scope();
      } else {
        this.$emit("select-entity", {entity: this.entity.name});
      }
    },

    is_disabled() {
      for (var i = 0; i < this.entity.type.length; i ++) {
        var el = this.entity.type[i];

        if (el == "Disabled") {
          return true;
        }
      }
      
      return false;
    },    

    css() {
      var result = "entity-tree-item";

      if (this.is_disabled()) {
        result += " entity-tree-item-disabled";
      }

      if (!this.current) {
        return result;
      }

      var entity = this.entity.name;
      var current;

      if (this.current.indexOf(".") != -1) {
        var pos = this.current.lastIndexOf(".") + 1;
        current = this.current.substr(pos, this.current.length - pos);
      } else {
        current = this.current;
      }
      
      if (entity === current) {
        result += " entity-tree-item-active";
      }

      return result;
    },

    kind() {
      var component = "";

      for (var i = 0; i < this.entity.type.length; i ++) {
        var el = this.entity.type[i];

        if (el == "Module") {
          component = el;
          break;
        } else if (el == "Component") {
          component = el;
          // Don't break, entity can also be module
        } else if (el == "Prefab") {
          component = el;
          break;
        } else if (el == "flecs.system.System") {
          component = "System";
          break;
        } else if (el == "Type") {
          component = el;
          break;
        }
      }

      return component;
    }
  },
  computed: {
    entity_id: function() {
      var text = this.text ? this.text : "";
      var entity = "";

      if (this.entity) {
        entity = this.entity.name ? this.entity.name : "";
      }

      // A bit of a convoluted way to makes sure that we show text instead of an
      // entity name when provided, except when the text is "." in which case we
      // show the entity name when there is one.
      if (text.length && (text != "." || !entity.length)) {
        return text;
      } else {
        return entity;
      }
    },
    show_nav() {
      return !this.nav_only && this.text != ".";
    }
  },
  template: `
    <div :class="css()" v-on:click.stop="single_click_nav_scope">
      <div>
        <img src="images/nav.png" class="entity-remove-icon" v-on:click.stop="nav_scope" v-if="show_nav && entity.child_count">
        <entity-kind :kind="kind()" v-if="entity_id.length"></entity-kind>
        <span class="entity-item-name">{{entity_id}}</span>
      </div>
    </div>
    `
});
