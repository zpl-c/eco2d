
Vue.component('entity-editor', {
  data: function() {
    return {
      entity_pending: "",
      entity: "",
      components: {},
    }
  },
  methods: {
    entity_url(entity) {
      return entity.replace(/\./g, "/");
    },
    request_entity(entity) {
      if (!entity || !entity.length) {
        this.entity_pending = "";
        this.entity = "";
        this.components = {};
        return;
      }

      this.entity_pending = entity;
      var entity_url = this.entity_url(entity);

      app.get("entity/" + entity_url, (msg) => {
        this.entity = this.entity_pending;
        this.components = msg;
        this.error = false;
      }, (Http) => {
        if (Http.status == 404) {
          // Entity can no longer be found on URL
          this.entity_pending = "";
          this.entity = "";
          this.components = {};
        }
      });
    },
    select_entity(entity) {
      if (entity != this.entity) {
        this.entity_pending = entity;
        this.request_entity(entity);
      }
    },    
    entity_hidden() {
      if (this.entity) {
        return 'visible;';
      } else {
        return 'hidden;';
      }
    },
    components_hidden() {
      if (this.components) {
        return 'visible;';
      } else {
        return 'hidden;';
      }
    },
    e_add(event) {
      this.$emit('select-component', {
        callback: function(component) {
          app.put("entity/" + this.entity_url(this.entity) + "?select=" + component, undefined, (msg) => {
            this.request_entity(this.entity);
          });
        }.bind(this)
      });
    },    
    e_sync(event) {
      this.request_entity(this.entity);
    },
    e_remove_component(event) {
      app.delete("entity/" + this.entity_url(this.entity) + "?select=" + event.component, undefined, (msg) => {
        this.request_entity(this.entity);
      });
    },
    e_delete(event) {
      app.delete("entity/" + this.entity_url(this.entity), undefined, (msg) => {
        this.request_entity(undefined);
      });
    }    
  },
  computed: {
    tags: function() {      
      var type = this.components.type;
      if (!type) {
        return [];
      }

      var data = this.components.data;
      var tags = [];
      for (var i = 0; i < type.length; i ++) {
        var tag = type[i];
        if (!data.hasOwnProperty(tag)) {
          if (Array.isArray(tag)) {
            tags.push(type[i].join(" | "));
          } else {
            tags.push(tag);
          }
        }
      }

      return tags;
    }
  },
  template: `
    <div>
      <div class="entity-editor" :style="'visibility: ' + entity_hidden()">
        <div class="entity-name">
          {{ components.path }}
          <img src="images/sync.png" class="entity-icon" v-on:click="e_sync">
          <img src="images/add.png" class="entity-icon" v-on:click="e_add">
          <img src="images/delete.png" class="entity-remove-icon" v-on:click="e_delete">
        </div>

        <div v-for="(value, key) in components.data" :style="'visibility: ' + components_hidden()">
          <entity-component :name="key" :value="value" v-on:remove_component="e_remove_component">
          </entity-component>
        </div>

        <div class="entity-component-name" v-if="tags.length">Tags</div>
        <div class="entity-tags">
          <div v-for="tag in tags" :style="'visibility: ' + components_hidden()">
            <entity-tag :name="tag" v-on:remove_component="e_remove_component"></entity-tag>
          </div>
        </div>
      </div>
    </div>
    `
});
